/*
  ==============================================================================
    FILE: TrackComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Complete Track UI header with real-time BE callbacks.
    UPDATED: Fix barev čísel tracků a synchronizace FX dat.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>

enum class InstrumentType
{
  Piano,
  Guitar,
  Bass,
  Drums,
  Synth,
  Other
};

class TrackComponent : public juce::Component
{
public:
  TrackComponent(int trackNumber, const juce::String &instrumentName, InstrumentType type);
  ~TrackComponent() override;

  // --- CALLBACKY PRO BACKEND ---
  std::function<void(int track, bool isActive)> onMuteChanged;
  std::function<void(int track, bool isActive)> onSoloChanged;
  std::function<void(int track, int value)> onVolumeChanged;
  std::function<void(int track, int value)> onPanChanged;
  std::function<void(int track, int value)> onReverbChanged;
  std::function<void(int track, int value)> onChorusChanged;

  // --- AKTUALIZACE STAVU ---
  void updateVolume(int newVolume);
  void updateMuteState(bool isMuted);
  void updateSoloState(bool isSoloed);
  void updateFxData(int pan, int reverb, int chorus);

  void setInstrument(const juce::String &name, juce::Colour colour);
  void setIcons(const juce::String &mute, const juce::String &solo, const juce::String &third);

  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  void showFxPopup();

  // --- DATA ---
  int trackNum;
  juce::String trackName;
  InstrumentType instrType;

  int currentVolume = 100;
  int currentPan = 64;
  int currentReverb = 0;
  int currentChorus = 0;

  bool isMuted = false;
  bool isSoloed = false;

  // --- BARVA ČÍSLA ---
  // Tady nastavíme tvou oranžovou/zlatou barvu pro text čísla
  juce::Colour trackNumberTextColor = juce::Colour(0xffff9000);

  // --- UI KOMPONENTY ---
  juce::TextButton trackNumberButton; // To je to tlačítko vlevo s číslem
  juce::Label nameLabel;
  juce::Slider volumeSlider;
  juce::TextButton muteButton, soloButton, thirdButton, fxButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};