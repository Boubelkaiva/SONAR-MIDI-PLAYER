/*
  ==============================================================================
    FILE: TrackComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Complete Track UI header with real-time BE callbacks.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>

// --- ENUM ---
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

  // --- REAL-TIME CALLBACKY PRO BACKEND (BE) ---
  // Logické stavy (True/False)
  std::function<void(int track, bool isActive)> onMuteChanged;
  std::function<void(int track, bool isActive)> onSoloChanged;

  // MIDI Kontroléry (0-127) - Okamžitě se posílají do Synth v reálném čase
  std::function<void(int track, int value)> onVolumeChanged; // CC 7
  std::function<void(int track, int value)> onPanChanged;    // CC 10
  std::function<void(int track, int value)> onReverbChanged; // CC 91
  std::function<void(int track, int value)> onChorusChanged; // CC 93

  // --- FUNKCE PRO AKTUALIZACI STAVU Z BACKENDU ---
  // Používá se při analýze MIDI metadat nebo při dálkové změně
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

  // --- INTERNÍ DATA (Pro synchronizaci UI) ---
  int trackNum;
  juce::String trackName;
  InstrumentType instrType;

  int currentVolume = 100;
  int currentPan = 64;
  int currentReverb = 0;
  int currentChorus = 0;

  bool isMuted = false;
  bool isSoloed = false;

  // --- UI KOMPONENTY ---
  juce::TextButton trackNumberButton;
  juce::Label nameLabel;
  juce::Slider volumeSlider;

  juce::TextButton muteButton, soloButton, thirdButton, fxButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};