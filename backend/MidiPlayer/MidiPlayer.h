/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine with Mute/Solo logic and Real-time CC support.
    UPDATED: Překlopeno na FluidSynth při zachování původní logiky.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <fluidsynth.h> // Nové srdce
#include "../MidiMapper/MidiMapper.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"

class MidiPlayer : public juce::AudioSource // Přidáno pro standardní JUCE streaming
{
public:
    MidiPlayer();
    ~MidiPlayer();

    // --- AUDIO ZÁKLAD (Zachováno) ---
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    // --- SPRÁVA DAT (Zachováno) ---
    void loadSoundFont(const juce::File &sf2File);
    void loadMidiFile(const juce::File &midiFile);
    void applyAnalysisResults(const std::vector<TrackData> &results);

    // --- TRANSPORT (Zachováno) ---
    void play();
    void stop();
    void pause();
    void setMasterVolume(float vol);

    // --- REAL-TIME OVLÁDÁNÍ (TTS-1 STYLE - Zachováno) ---
    void sendRealTimeControlChange(int trackNum, int controller, int value);

    // --- OVLÁDÁNÍ MUTE / SOLO (Zachováno) ---
    void setChannelMute(int trackIdx, bool shouldMute);
    void setChannelSolo(int trackIdx, bool shouldSolo);
    bool isChannelAudible(int channel) const;

    // --- GETTERY PRO UI (Zachováno) ---
    MidiMapper *getMapper() const { return mapper.get(); }
    juce::AudioDeviceManager &getDeviceManager() { return const_cast<juce::AudioDeviceManager &>(deviceManager); }

private:
    void processMidiMessage(const juce::MidiMessage &m);

    // FluidSynth objekty místo tsf
    fluid_settings_t *settings = nullptr;
    fluid_synth_t *synth = nullptr;

    juce::AudioDeviceManager deviceManager;
    double currentSampleRate = 48000.0;

    // MIDI stav pro každý z 16 kanálů
    int currentBankMSB[16];
    int currentBankLSB[16];

    // --- STAVY PRO MUTE A SOLO (Přesně podle tvého vzoru) ---
    bool channelMuted[16];
    bool channelSolo[16];

    MidiMode currentMode = MidiMode::GM;
    juce::MidiMessageSequence midiSequence;
    double playheadSeconds = 0.0;
    bool isPlaying = false;
    float masterVolume = 1.0f;

    std::unique_ptr<MidiMapper> mapper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPlayer)
};