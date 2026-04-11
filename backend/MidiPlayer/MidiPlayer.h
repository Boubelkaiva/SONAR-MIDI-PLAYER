/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine using TinySoundFont with full MIDI mapping support.
    UPDATED: Added MidiAnalyzer include for TrackData type safety.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MidiMapper/MidiMapper.h"
#include "../MidiAnalyzer/MidiAnalyzer.h" // KLÍČOVÉ: Aby kompilátor znal strukturu TrackData

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

  // Nyní je TrackData správně rozpoznán díky includu výše
  void applyAnalysisResults(const std::vector<TrackData> &results);

  void play();
  void stop();
  void pause();
  void setMasterVolume(float vol);

  MidiMapper *getMapper() const { return mapper.get(); }

private:
  void processMidiMessage(const juce::MidiMessage &m);

  // TSF Instance a Audio data
  struct tsf *g_tinyfont = nullptr;
  double currentSampleRate = 44100.0;

  // Pomocný buffer pro renderování TSF (stereo interleaved)
  juce::AudioBuffer<float> renderBuffer;

  // MIDI stav pro každý z 16 kanálů
  int currentBankMSB[16];
  int currentBankLSB[16];
  MidiMode currentMode = MidiMode::GM;

  // Časování a sekvence
  juce::MidiMessageSequence midiSequence;
  double playheadSeconds = 0.0;
  bool isPlaying = false;
  float masterVolume = 1.0f;

  std::unique_ptr<MidiMapper> mapper;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPlayer)
};