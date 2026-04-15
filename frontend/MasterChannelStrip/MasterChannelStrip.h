/*
  ==============================================================================
    FILE: MasterChannelStrip.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Modal panel for AI Master Effects (Sonar Style)
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../backend/MasterEffects/MasterEffects.h"

class MasterChannelStrip : public juce::Component
{
public:
    MasterChannelStrip(MasterEffects &effects);
    ~MasterChannelStrip() override = default;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    MasterEffects &masterEffects;

    // --- Ovládací prvky ---
    juce::Slider lowCutKnob;
    juce::Slider compThresholdKnob;
    juce::Slider compRatioKnob;
    juce::Slider limiterKnob;

    juce::Label titleLabel{"title", "AI MASTER PROCESSOR"};
    juce::Label lowCutLabel{"lcL", "LOW CUT"};
    juce::Label compLabel{"cpL", "S-TYPE COMP"};
    juce::Label limLabel{"lmL", "MAX LIMITER"};

    // Stylizace knoflíku
    void applySonarStyle(juce::Slider &s, const juce::String &unit);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterChannelStrip)
};