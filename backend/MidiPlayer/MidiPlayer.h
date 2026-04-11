/*
  ==============================================================================
    FILE: MidiPlayer.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine using TinySoundFont with full MIDI mapping support.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../MidiMapper/MidiMapper.h"

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

  // --- OPRAVA CHYB: Deklarace chybějících členů ---
  juce::AudioBuffer<float> renderBuffer; // Pro vnitřní výpočet TSF

  // MIDI stav
  int currentBankMSB[16]; // Sledování bank pro každý kanál
  int currentBankLSB[16];
  MidiMode currentMode = MidiMode::GM; // Výchozí mód

  // Časování a sekvence
  juce::MidiMessageSequence midiSequence;
  double playheadSeconds = 0.0;
  bool isPlaying = false;
  float masterVolume = 1.0f;

  std::unique_ptr<MidiMapper> mapper;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPlayer)
};