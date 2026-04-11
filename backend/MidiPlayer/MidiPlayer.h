/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine with Mute/Solo logic support.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MidiMapper/MidiMapper.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"

struct tsf;

class MidiPlayer
{
public:
  MidiPlayer();
  ~MidiPlayer();

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
  void releaseResources();

  void loadSoundFont(const juce::File &sf2File);
  void loadMidiFile(const juce::File &midiFile);

  void applyAnalysisResults(const std::vector<TrackData> &results);

  void play();
  void stop();
  void pause();
  void setMasterVolume(float vol);

  // --- OVLÁDÁNÍ MUTE / SOLO ---
  void setChannelMute(int channel, bool shouldMute)
  {
    if (channel >= 0 && channel < 16)
      channelMuted[channel] = shouldMute;
  }

  void setChannelSolo(int channel, bool shouldSolo)
  {
    if (channel >= 0 && channel < 16)
      channelSolo[channel] = shouldSolo;
  }

  // Pomocná funkce pro zjištění, zda má kanál hrát (použijeme v .cpp)
  bool isChannelAudible(int channel) const;

  // --- GETTERY PRO UI ---
  MidiMapper *getMapper() const { return mapper.get(); }
  juce::AudioDeviceManager &getDeviceManager() { return deviceManager; }

private:
  void processMidiMessage(const juce::MidiMessage &m);

  juce::AudioDeviceManager deviceManager;

  struct tsf *g_tinyfont = nullptr;
  double currentSampleRate = 44100.0;
  juce::AudioBuffer<float> renderBuffer;

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