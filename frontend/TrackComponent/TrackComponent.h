/*
  ==============================================================================
    FILE: TrackComponent.h
    PROJECT: SONAR MIDI PLAYER
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

    // --- CALLBACKY ---
    std::function<void(int, bool)> onMuteChanged;
    std::function<void(int, bool)> onSoloChanged;
    std::function<void(int, float)> onVolumeChanged;
    std::function<void(int, float)> onPanChanged;
    std::function<void(int, float)> onReverbChanged;
    std::function<void(int, float)> onChorusChanged;

    void updateVolume(float newVolume);
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

    // --- UI ---
    juce::TextButton trackNumberButton;
    juce::Label nameLabel;
    juce::Slider volumeSlider;

    juce::Slider panSlider, reverbSlider, chorusSlider;
    juce::TextButton muteButton, soloButton, thirdButton, fxButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};