/*
  ==============================================================================
    FILE: VUMeter.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Vertikální VU metr pro indikaci MIDI aktivity tracku.
    AUTHOR: AI (Iva's Assistant)
  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class VUMeter : public juce::Component, public juce::Timer
{
public:
    VUMeter();
    ~VUMeter() override;

    void paint(juce::Graphics &g) override;
    void timerCallback() override;

    /** Nastaví úroveň metru na základě velocity (0-127) */
    void trigger(int velocity);

private:
    float currentLevel = 0.0f;
    const float decayFactor = 0.88f; // Jak rychle padá hladina dolů

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeter)
};