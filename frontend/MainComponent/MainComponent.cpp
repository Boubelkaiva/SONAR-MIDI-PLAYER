/*
  ==============================================================================
    FILE: MainComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Main container - Async loading to prevent UI freezing.
    FIXED: Thread safety during MIDI loading and analysis.
  ==============================================================================
*/

#include "MainComponent.h"
#include "../../backend/MidiAnalyzer/MidiAnalyzer.h"
#include "../../backend/MidiMapper/MidiMapper.h"
#include <iostream>

MainComponent::MainComponent()
{
    // 1. Inicializace Backend modulů
    midiPlayer = std::make_unique<MidiPlayer>();
    bankManager = std::make_unique<BankManager>();

    // 2. Inicializace UI komponent
    trackPanel = std::make_unique<TrackPanelComponent>();
    addAndMakeVisible(*trackPanel);

    transport = std::make_unique<TransportComponent>(*midiPlayer);
    addAndMakeVisible(*transport);

    sf2List = std::make_unique<SF2ListComponent>(*bankManager);
    addAndMakeVisible(*sf2List);

    masterPanel = std::make_unique<MasterPanel>();
    addAndMakeVisible(*masterPanel);

    setSize(1200, 800);

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
            {
                // Load banky může být taky náročný, ale SF2 obvykle držíme v RAM
                midiPlayer->loadSoundFont(selectedFile);
            }
        };
    }

    if (masterPanel)
    {
        masterPanel->onMidiFileSelected = [this](const juce::File &midiFile)
        {
            if (midiFile.existsAsFile())
            {
                std::cout << "\n[Main] Start asynchronního načítání: " << midiFile.getFileName().toRawUTF8() << std::endl;

                // --- ASYNC BLOK: Tohle vyřeší to točící se kolečko ---
                juce::Thread::launch([this, midiFile]()
                                     {
                    // 1. Analýza na pozadí
                    MidiAnalyzer analyzer;
                    MidiMapper* currentMapper = (midiPlayer != nullptr) ? midiPlayer->getMapper() : nullptr;
                    
                    auto trackMetadata = analyzer.analyzeFile(midiFile, currentMapper);

                    // 2. Aktualizace UI (musí se poslat zpět do hlavního vlákna)
                    juce::MessageManager::callAsync([this, trackMetadata]() 
                    {
                        for (const auto &data : trackMetadata)
                        {
                            trackPanel->updateTrackFromMetadata(data.channel - 1, data.instrumentName, data.initialVolume);
                        }
                    });

                    // 3. Načtení do playeru (zabezpečeno zámkem uvnitř playeru)
                    if (midiPlayer)
                        midiPlayer->loadMidiFile(midiFile);

                    std::cout << "[Main] Načítání dokončeno na pozadí." << std::endl; });
            }
        };

        masterPanel->onVolumeChanged = [this](float volume)
        {
            if (midiPlayer)
                midiPlayer->setMasterVolume(volume);
        };
    }

    // --- 4. DEBUG STARTUP (AUTO-LOAD) ---
    const std::vector<juce::File> &foundBanks = bankManager->getLoadedBanks();
    if (!foundBanks.empty() && midiPlayer)
    {
        midiPlayer->loadSoundFont(foundBanks[0]);
    }

    setAudioChannels(0, 2);
}

MainComponent::~MainComponent() { shutdownAudio(); }

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

void MainComponent::paint(juce::Graphics &g) { g.fillAll(juce::Colour(0xff181818)); }

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