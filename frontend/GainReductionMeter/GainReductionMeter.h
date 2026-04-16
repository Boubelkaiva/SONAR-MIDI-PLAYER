/*
  ==============================================================================
    FILE: GainReductionMeter.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Komponenta pro vizualizaci útlumu (Gain Reduction) kompresoru.
    AUTHOR: Gemini Code Assist
  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class GainReductionMeter : public juce::Component, public juce::Timer
{
public:
    GainReductionMeter();
    ~GainReductionMeter() override;

    void paint(juce::Graphics &g) override;
    void timerCallback() override;

    void setReduction(float reduction);

private:
    float currentReduction = 0.0f;
    const float fallSpeed = 0.15f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainReductionMeter)
};