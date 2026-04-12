/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine with Mute/Solo logic and Real-time CC support.
    UPDATED: Integrated Mute/Solo state arrays and proper method signatures.
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

  // --- AUDIO ZÁKLAD ---
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
  void releaseResources();

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
  /** Posílá CC zprávy přímo do syntézy (Volume=7, Pan=10, Reverb=91, Chorus=93) */
  void sendRealTimeControlChange(int trackNum, int controller, int value);

  // --- OVLÁDÁNÍ MUTE / SOLO ---
  /** Nastaví Mute pro kanál (0-15) a okamžitě utne hrající hlasy v .cpp */
  void setChannelMute(int trackIdx, bool shouldMute);

  /** Nastaví Solo pro kanál (0-15) */
  void setChannelSolo(int trackIdx, bool shouldSolo);

  /** Pomocná funkce pro vyhodnocení, zda má kanál produkovat zvuk (zohledňuje Mute i Solo) */
  bool isChannelAudible(int channel) const;

  // --- GETTERY PRO UI ---
  MidiMapper *getMapper() const { return mapper.get(); }
  juce::AudioDeviceManager &getDeviceManager() { return deviceManager; }

private:
  void processMidiMessage(const juce::MidiMessage &m);

  juce::AudioDeviceManager deviceManager;

  struct tsf *g_tinyfont = nullptr;
  double currentSampleRate = 48000.0;
  juce::AudioBuffer<float> renderBuffer;

  // MIDI stav pro každý z 16 kanálů (pro přepínání bank)
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