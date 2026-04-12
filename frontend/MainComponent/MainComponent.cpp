/*
  ==============================================================================
    FILE: MainComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Main container - Async loading and Audio HW synchronization.
    FIXED: Automatické přepínání výstupu při změně zařízení.
  ==============================================================================
*/

#include "MainComponent.h"
#include <iostream>

MainComponent::MainComponent()
{
    // 1. Inicializace Backend modulů
    midiPlayer = std::make_unique<MidiPlayer>();
    bankManager = std::make_unique<BankManager>();

    // --- SYNCHRONIZACE AUDIO HARDWARE ---
    auto &dm = midiPlayer->getDeviceManager();
    dm.addChangeListener(this);
    dm.initialise(0, 2, nullptr, true);

    // Inicializace výchozích kanálů
    setAudioChannels(0, 2);

    // 2. Inicializace UI komponent
    trackPanel = std::make_unique<TrackPanelComponent>();
    addAndMakeVisible(*trackPanel);

    transport = std::make_unique<TransportComponent>(*midiPlayer);
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
                std::cout << "[Main] Async load: " << midiFile.getFileName() << std::endl;

                juce::Thread::launch([this, midiFile]()
                                     {
                    MidiAnalyzer analyzer;
                    MidiMapper* currentMapper = (midiPlayer != nullptr) ? midiPlayer->getMapper() : nullptr;
                    auto trackMetadata = analyzer.analyzeFile(midiFile, currentMapper);

                    juce::MessageManager::callAsync([this, trackMetadata]() 
                    {
                        for (const auto &data : trackMetadata)
                            trackPanel->updateTrackFromMetadata(data.channel - 1, data.instrumentName, data.initialVolume);
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

    // --- PROPOJENÍ TRACK PANELU S MIDI PLAYEREM (REAL-TIME FX) ---
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
        auto *device = midiPlayer->getDeviceManager().getCurrentAudioDevice();
        if (device != nullptr)
        {
            // Informujeme player o nové vzorkovací frekvenci
            midiPlayer->prepareToPlay(device->getCurrentBufferSizeSamples(),
                                      device->getCurrentSampleRate());

            // DŮLEŽITÉ: Vynutíme restart audio streamu pro nové zařízení
            setAudioChannels(0, 2);

            std::cout << "[AI SYSTEM] Zvukové zařízení změněno na: " << device->getName()
                      << " (" << device->getCurrentSampleRate() << " Hz)" << std::endl;
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
    if (midiPlayer)
        midiPlayer->getNextAudioBlock(bufferToFill);
    else
        bufferToFill.clearActiveBufferRegion();
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

    if (transport)
        transport->setBounds(rightArea.removeFromTop(60));

    if (sf2List)
        sf2List->setBounds(rightArea);

    if (masterPanel)
        masterPanel->setBounds(area.removeFromBottom(100));

    if (trackPanel)
        trackPanel->setBounds(area);
}