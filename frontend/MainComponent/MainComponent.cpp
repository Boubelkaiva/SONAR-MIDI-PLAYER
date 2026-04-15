/*
  ==============================================================================

    FILE: MainComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Main container - DEBUG AUDIO PIPELINE TRACE VERSION
    FIXED: audio routing + controlled audio callback logging (NO SPAM)

  ==============================================================================
*/

#include "MainComponent.h"
#include "Icons.h"
#include <iostream>
#include <atomic>

#define LOG(x) std::cout << x << std::endl

// =========================
// GLOBAL DEBUG CONTROL
// =========================
static std::atomic<int> audioCallbackCounter{0};
static constexpr int AUDIO_LOG_EVERY_N_BLOCKS = 200; // throttle log

MainComponent::MainComponent()
{
    LOG("[MAIN] CONSTRUCTOR START");

    // =========================
    // BACKEND
    // =========================
    midiPlayer = std::make_unique<MidiPlayer>();
    bankManager = std::make_unique<BankManager>();

    // =========================
    // AUDIO PIPELINE
    // =========================
    LOG("[AUDIO] init JUCE audio channels");
    setAudioChannels(0, 2);

    // =========================
    // UI
    // =========================
    trackPanel = std::make_unique<TrackPanelComponent>();
    addAndMakeVisible(*trackPanel);

    transport = std::make_unique<TransportComponent>(*midiPlayer, *bankManager);
    addAndMakeVisible(*transport);

    masterPanel = std::make_unique<MasterPanel>();
    addAndMakeVisible(*masterPanel);

    // =========================
    // TRANSPORT
    // =========================
    if (transport)
    {
        transport->onStartClicked = [this]()
        {
            LOG("[UI] PLAY CLICKED");
            if (midiPlayer)
                midiPlayer->play();
        };

        transport->onStopClicked = [this]()
        {
            LOG("[UI] STOP CLICKED");
            if (midiPlayer)
                midiPlayer->stop();
        };
    }

    // =========================
    // MIDI LOAD FLOW
    // =========================
    if (masterPanel)
    {
        masterPanel->onMidiFileSelected = [this](const juce::File &midiFile)
        {
            LOG("[UI] MIDI FILE SELECTED");

            if (!midiFile.existsAsFile())
            {
                LOG("[MIDI] FILE NOT FOUND");
                return;
            }

            juce::Thread::launch([this, midiFile]()
                                 {
                LOG("[MIDI] ANALYSIS THREAD START");

                MidiAnalyzer analyzer;
                MidiMapper* currentMapper =
                    (midiPlayer != nullptr) ? midiPlayer->getMapper() : nullptr;

                auto trackMetadata =
                    analyzer.analyzeFile(midiFile, currentMapper);

                LOG("[MIDI] ANALYSIS DONE");

                juce::MessageManager::callAsync([this, trackMetadata]()
                {
                    LOG("[UI] APPLY TRACK METADATA");

                    for (const auto &data : trackMetadata)
                    {
                        int trackIdx = data.channel - 1;

                        trackPanel->updateTrackFromMetadata(
                            trackIdx,
                            data.instrumentName,
                            data.initialVolume
                        );

                        trackPanel->setTrackFxData(
                            trackIdx,
                            64,
                            (int)data.initialReverb,
                            (int)data.initialChorus
                        );

                        if (midiPlayer)
                        {
                            midiPlayer->sendRealTimeControlChange(data.channel, 91, (int)data.initialReverb);
                            midiPlayer->sendRealTimeControlChange(data.channel, 93, (int)data.initialChorus);
                        }
                    }

                    LOG("[UI] METADATA APPLIED");
                });

                if (midiPlayer)
                {
                    LOG("[MIDI] LOAD INTO PLAYER");
                    midiPlayer->loadMidiFile(midiFile);
                } });
        };

        masterPanel->onVolumeChanged = [this](float volume)
        {
            if (midiPlayer)
                midiPlayer->setMasterVolume(volume);
        };
    }

    // =========================
    // TRACK CONTROLS
    // =========================
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
            LOG("[UI] MUTE ch=" << trk << " val=" << muted);
            if (midiPlayer)
                midiPlayer->setChannelMute(trk, muted);
        };

        trackPanel->onTrackSoloChanged = [this](int trk, bool soloed)
        {
            LOG("[UI] SOLO ch=" << trk << " val=" << soloed);
            if (midiPlayer)
                midiPlayer->setChannelSolo(trk, soloed);
        };
    }

    setSize(1200, 800);

    LOG("[MAIN] CONSTRUCTOR END");
}

MainComponent::~MainComponent()
{
    LOG("[MAIN] DESTRUCTOR");
    shutdownAudio();
}

// =========================
// AUDIO DEVICE CHANGE
// =========================
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (midiPlayer && source == &midiPlayer->getDeviceManager())
    {
        auto &dm = midiPlayer->getDeviceManager();
        auto *device = dm.getCurrentAudioDevice();

        if (device != nullptr)
        {
            LOG("[AUDIO] DEVICE OK: " + device->getName().toStdString());

            midiPlayer->prepareToPlay(
                device->getCurrentBufferSizeSamples(),
                device->getCurrentSampleRate());

            LOG("[AUDIO] SYNTH REPREPARED");
        }
    }
}

// =========================
// AUDIO CALLBACK (FIXED LOG SPAM)
// =========================
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    // throttle log (otherwise UI floods + lag)
    int c = ++audioCallbackCounter;
    bool logThis = (c % AUDIO_LOG_EVERY_N_BLOCKS == 0);

    if (logThis)
        LOG("[AUDIO] CALLBACK START");

    bufferToFill.clearActiveBufferRegion();

    if (!midiPlayer)
    {
        if (logThis)
            LOG("[AUDIO] NO PLAYER");
        return;
    }

    midiPlayer->getNextAudioBlock(bufferToFill);

    if (logThis)
        LOG("[AUDIO] CALLBACK END");
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    LOG("[AUDIO] PREPARE TO PLAY");

    if (midiPlayer)
        midiPlayer->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::releaseResources()
{
    LOG("[AUDIO] RELEASE RESOURCES");
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff181818));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    if (transport)
        transport->setBounds(area.removeFromRight(320));

    if (masterPanel)
        masterPanel->setBounds(area.removeFromBottom(110).reduced(5));

    if (trackPanel)
        trackPanel->setBounds(area.reduced(5));
}

void MainComponent::parentHierarchyChanged()
{
    LOG("[MAIN] parentHierarchyChanged (ignored)");
}