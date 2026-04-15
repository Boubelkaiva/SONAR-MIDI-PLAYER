/*
  ==============================================================================
    FILE: MainComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Main container - Async loading and Audio HW synchronization.
    UPDATED: AI - Obnoveno propojení audio bloku pro funkční zvuk.
    FIXED: Správné předání MidiPlayer do TransportComponent.
  ==============================================================================
*/

#include "MainComponent.h"
#include "Icons.h"
#include <iostream>

MainComponent::MainComponent()
{
    // 1. Inicializace AI Backend modulů
    midiPlayer = std::make_unique<MidiPlayer>();
    bankManager = std::make_unique<BankManager>();

    // --- BEZPEČNOSTNÍ KONTROLA ---
    jassert(midiPlayer != nullptr);

    // --- SYNCHRONIZACE AUDIO HARDWARE ---
    auto &dm = midiPlayer->getDeviceManager();
    dm.addChangeListener(this);
    dm.initialise(0, 2, nullptr, true);

    // Inicializace výchozích kanálů pro AI
    setAudioChannels(0, 2);

    // 2. Inicializace UI komponent
    trackPanel = std::make_unique<TrackPanelComponent>();
    addAndMakeVisible(*trackPanel);

    // 🔥 KLÍČOVÁ FIXACE – předání reference
    transport = std::make_unique<TransportComponent>(*midiPlayer, *bankManager);
    addAndMakeVisible(*transport);

    sf2List = std::make_unique<SF2ListComponent>(*bankManager);
    addAndMakeVisible(*sf2List);

    masterPanel = std::make_unique<MasterPanel>();
    addAndMakeVisible(*masterPanel);

    // --- 3. CALLBACKY A PROPOJENÍ ---
    if (transport)
    {
        transport->onStartClicked = [this]()
        {
            if (midiPlayer)
                midiPlayer->play();
        };

        transport->onStopClicked = [this]()
        {
            if (midiPlayer)
                midiPlayer->stop();
        };
    }

    if (sf2List)
    {
        sf2List->onSelectionChanged = [this](const juce::File &selectedFile)
        {
            if (selectedFile.existsAsFile() && midiPlayer)
                midiPlayer->loadSoundFont(selectedFile);
        };
    }

    if (masterPanel)
    {
        masterPanel->onMidiFileSelected = [this](const juce::File &midiFile)
        {
            if (midiFile.existsAsFile())
            {
                std::cout << "[AI] Async load: " << midiFile.getFileName() << std::endl;

                juce::Thread::launch([this, midiFile]()
                                     {
                    MidiAnalyzer analyzer;
                    MidiMapper* currentMapper = (midiPlayer != nullptr) ? midiPlayer->getMapper() : nullptr;
                    auto trackMetadata = analyzer.analyzeFile(midiFile, currentMapper);

                    juce::MessageManager::callAsync([this, trackMetadata]()
                    {
                        std::cout << "--- [AI] Syncing metadata to UI ---" << std::endl;

                        for (const auto &data : trackMetadata)
                        {
                            int trackIdx = data.channel - 1;

                            std::cout << "[AI] Track " << data.channel
                                      << " -> R:" << (int)data.initialReverb
                                      << " C:" << (int)data.initialChorus << std::endl;

                            // UI
                            trackPanel->updateTrackFromMetadata(trackIdx, data.instrumentName, data.initialVolume);
                            trackPanel->setTrackFxData(trackIdx, 64, (int)data.initialReverb, (int)data.initialChorus);

                            // AUDIO
                            if (midiPlayer)
                            {
                                midiPlayer->sendRealTimeControlChange(data.channel, 91, (int)data.initialReverb);
                                midiPlayer->sendRealTimeControlChange(data.channel, 93, (int)data.initialChorus);
                            }
                        }

                        std::cout << "--- [AI] Sync complete ---" << std::endl;
                    });

                    if (midiPlayer)
                        midiPlayer->loadMidiFile(midiFile); });
            }
        };

        masterPanel->onVolumeChanged = [this](float volume)
        {
            if (midiPlayer)
                midiPlayer->setMasterVolume(volume);
        };
    }

    if (trackPanel)
    {
        trackPanel->onTrackVolumeChanged = [this](int trk, int val)
        {
            if (midiPlayer)
                midiPlayer->sendRealTimeControlChange(trk + 1, 7, val);
        };

        trackPanel->onTrackPanChanged = [this](int trk, int val)
        {
            if (midiPlayer)
                midiPlayer->sendRealTimeControlChange(trk + 1, 10, val);
        };

        trackPanel->onTrackReverbChanged = [this](int trk, int val)
        {
            if (midiPlayer)
                midiPlayer->sendRealTimeControlChange(trk + 1, 91, val);
        };

        trackPanel->onTrackChorusChanged = [this](int trk, int val)
        {
            if (midiPlayer)
                midiPlayer->sendRealTimeControlChange(trk + 1, 93, val);
        };

        trackPanel->onTrackMuteChanged = [this](int trk, bool muted)
        {
            if (midiPlayer)
                midiPlayer->setChannelMute(trk, muted);
        };

        trackPanel->onTrackSoloChanged = [this](int trk, bool soloed)
        {
            if (midiPlayer)
                midiPlayer->setChannelSolo(trk, soloed);
        };
    }

    // --- 4. AUTO-LOAD PRVNÍ BANKY ---
    const std::vector<juce::File> &foundBanks = bankManager->getLoadedBanks();
    if (!foundBanks.empty() && midiPlayer)
        midiPlayer->loadSoundFont(foundBanks[0]);

    setSize(1200, 800);
}

MainComponent::~MainComponent()
{
    if (midiPlayer)
        midiPlayer->getDeviceManager().removeChangeListener(this);

    shutdownAudio();
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (midiPlayer && source == &midiPlayer->getDeviceManager())
    {
        auto &dm = midiPlayer->getDeviceManager();
        auto *device = dm.getCurrentAudioDevice();

        if (device != nullptr && !device->getName().containsIgnoreCase("none"))
        {
            double newSampleRate = device->getCurrentSampleRate();
            int newSamplesPerBlock = device->getCurrentBufferSizeSamples();

            midiPlayer->prepareToPlay(newSamplesPerBlock, newSampleRate);
            setAudioChannels(0, 2);

            std::cout << "[AI STABLE] Audio běží na: " << device->getName()
                      << " (" << newSampleRate << " Hz)" << std::endl;
        }
        else
        {
            std::cout << "[AI WATCHDOG] HW není dostupný, resetuji..." << std::endl;
            dm.initialiseWithDefaultDevices(0, 2);
        }
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (midiPlayer)
        midiPlayer->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (midiPlayer != nullptr)
        midiPlayer->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    if (midiPlayer)
        midiPlayer->releaseResources();
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff181818));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto rightArea = area.removeFromRight(300);

    // 👉 celý pravý panel řídí TransportComponent
    if (transport)
        transport->setBounds(rightArea);

    if (masterPanel)
        masterPanel->setBounds(area.removeFromBottom(100));

    if (trackPanel)
        trackPanel->setBounds(area);
}

void MainComponent::parentHierarchyChanged()
{
    if (getParentComponent() != nullptr)
    {
        std::cout << "[AI SYSTEM] Komponenta připojená, aktivuji audio..." << std::endl;
        setAudioChannels(0, 2);
    }
}