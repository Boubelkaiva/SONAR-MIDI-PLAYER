/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine s FluidSynth a podporou AI Master efektů.
    UPDATED: [2026-04-15] FIX: added missing bank helper
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <fluidsynth.h>
#include <functional>
#include <vector> // FIX: pro TrackData vector
#include "../MidiMapper/MidiMapper.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"
#include "../MasterEffects/MasterEffects.h"

class MidiPlayer : public juce::AudioSource
{
public:
  MidiPlayer();
  ~MidiPlayer();

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  void loadSoundFont(const juce::File &sf2File);
  void loadMidiFile(const juce::File &midiFile);
  void applyAnalysisResults(const std::vector<TrackData> &results);

  void play();
  void stop();
  void pause();
  void setMasterVolume(float vol);

  std::function<void(int channel, int velocity)> onMidiActivity;

  void sendRealTimeControlChange(int trackNum, int controller, int value);

  void setChannelMute(int trackIdx, bool shouldMute);
  void setChannelSolo(int trackIdx, bool shouldSolo);
  bool isChannelAudible(int channel) const;
  void sendProgramChange(int trackNum, int program);

  MidiMapper *getMapper() const { return mapper.get(); }
  MasterEffects &getMasterEffects() { return masterEffects; }
  juce::AudioDeviceManager &getDeviceManager() { return const_cast<juce::AudioDeviceManager &>(deviceManager); }

private:
  void processMidiMessage(const juce::MidiMessage &m);

  // 🔥 FIX: missing helper declaration
  int getFullBank(int chan);

  fluid_settings_t *settings = nullptr;
  fluid_synth_t *synth = nullptr;

  juce::AudioDeviceManager deviceManager;
  double currentSampleRate = 48000.0;

  MasterEffects masterEffects;

  juce::String lastSf2Path;

  int currentBankMSB[16];
  int currentBankLSB[16];

  bool channelMuted[16];
  bool channelSolo[16];

  juce::MidiMessageSequence midiSequence;
  double playheadSeconds = 0.0;
  bool isPlaying = false;
  int lastEventIndex = 0;
  float masterVolume = 1.0f;

  std::unique_ptr<MidiMapper> mapper;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPlayer)
};