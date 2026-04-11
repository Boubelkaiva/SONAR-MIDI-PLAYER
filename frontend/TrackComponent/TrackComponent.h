/*
  ==============================================================================

    FILE: TrackComponent.h
    PROJECT: SONAR MIDI PLAYER
    MODULE: frontend/TrackComponent
    DESCRIPTION: Header for a single MIDI track row.
                 FIXED: Explicitly declared updateVolume.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace InstrumentType
{
    enum Type
    {
        Piano,
        Guitar,
        Bass,
        Drums,
        Synth,
        Other
    };
}

class TrackComponent : public juce::Component
{
public:
    TrackComponent(int trackNumber, const juce::String &instrumentName, InstrumentType::Type type);
    ~TrackComponent() override;

    // Metody pro aktualizaci obsahu
    void setInstrument(const juce::String &name, juce::Colour colour);
    void setIcons(const juce::String &mute, const juce::String &solo, const juce::String &third);

    // AI: Tato metoda musí být deklarována zde
    void updateVolume(float newVolume);

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    int trackNum;
    juce::String trackName;
    InstrumentType::Type instrumentType;

    juce::TextButton trackNumberButton;
    juce::Label nameLabel;
    juce::Slider volumeSlider;
    juce::TextButton muteButton;
    juce::TextButton soloButton;
    juce::TextButton thirdButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackComponent)
};