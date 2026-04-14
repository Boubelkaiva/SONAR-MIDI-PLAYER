/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine s FluidSynth a podporou automatického restartu.
    UPDATED: Přidána proměnná lastSf2Path pro obnovu po změně Sample Rate.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <fluidsynth.h>
#include "../MidiMapper/MidiMapper.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"

class MidiPlayer : public juce::AudioSource
{
public:
  MidiPlayer();
  ~MidiPlayer();

  // --- AUDIO ZÁKLAD ---
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
  void releaseResources() override;

  // --- SPRÁVA DAT ---
  void loadSoundFont(const juce::File &sf2File);
  void loadMidiFile(const juce::File &midiFile);
  void applyAnalysisResults(const std::vector<TrackData> &results);

  // --- TRANSPORT ---
  void play();
  void stop();
  void pause();
  void setMasterVolume(float vol);

  // --- REAL-TIME OVLÁDÁNÍ (TTS-1 STYLE) ---
  void sendRealTimeControlChange(int trackNum, int controller, int value);

  // --- OVLÁDÁNÍ MUTE / SOLO ---
  void setChannelMute(int trackIdx, bool shouldMute);
  void setChannelSolo(int trackIdx, bool shouldSolo);
  bool isChannelAudible(int channel) const;

  // --- GETTERY PRO UI ---
  MidiMapper *getMapper() const { return mapper.get(); }
  juce::AudioDeviceManager &getDeviceManager() { return const_cast<juce::AudioDeviceManager &>(deviceManager); }

private:
  void processMidiMessage(const juce::MidiMessage &m);

  // FluidSynth objekty
  fluid_settings_t *settings = nullptr;
  fluid_synth_t *synth = nullptr;

  juce::AudioDeviceManager deviceManager;
  double currentSampleRate = 48000.0;

  // Pomocná proměnná pro obnovu zvuku při změně HW (repro/sluchátka)
  juce::String lastSf2Path;

  // MIDI stav pro každý z 16 kanálů
  int currentBankMSB[16];
  int currentBankLSB[16];

  // --- STAVY PRO MUTE A SOLO ---
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