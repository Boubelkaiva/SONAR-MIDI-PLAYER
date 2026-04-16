/*
  ==============================================================================
    FILE: TrackComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Complete Track UI header with real-time BE callbacks and VU Meter.
    UPDATED: [2026-04-15] Přidán VUMeter a trigger pro MIDI aktivitu.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>

// Dopředná deklarace, aby linker věděl o VUMeteru
class VUMeter;

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
  std::function<void(int track, int bank, int category, int program)> onInstrumentSelected;

  // --- AKTUALIZACE STAVU ---
  void updateVolume(int newVolume);
  void updateMuteState(bool isMuted);
  void updateSoloState(bool isSoloed);
  void updateFxData(int pan, int reverb, int chorus);

  /** Spustí vizuální indikaci na VU metru (volat při MIDI note on) */
  void triggerVuMeter(int velocity);

  void setInstrument(const juce::String &name, juce::Colour colour);
  void setIcons(const juce::String &mute, const juce::String &solo, const juce::String &third);

  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  void showFxPopup();
  void showInstrumentPopup(); // 🔥 NOVÉ

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
  juce::Colour trackNumberTextColor = juce::Colour(0xffff9000);

  // --- UI KOMPONENTY ---
  juce::TextButton trackNumberButton;
  juce::Label nameLabel;

  // Samostatná komponenta pro VU metr (vlepeno mezi název a slider)
  std::unique_ptr<VUMeter> vuMeter;

  juce::Slider volumeSlider;
  juce::TextButton muteButton, soloButton, thirdButton, fxButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};