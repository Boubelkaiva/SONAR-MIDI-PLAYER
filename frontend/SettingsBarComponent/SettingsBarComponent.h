/*
  ==============================================================================
    FILE: SettingsBarComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Samostatná komponenta pro ovládání efektů a hardware.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../backend/MidiPlayer/MidiPlayer.h"

class SettingsBarComponent : public juce::Component
{
public:
    SettingsBarComponent(MidiPlayer &player);
    ~SettingsBarComponent() override = default;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    MidiPlayer &midiPlayer;

    juce::TextButton fxButton;
    juce::TextButton settingsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsBarComponent)
};