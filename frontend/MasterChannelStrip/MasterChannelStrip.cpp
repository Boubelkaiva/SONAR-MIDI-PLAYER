/*
  ==============================================================================
    FILE: MasterChannelStrip.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Ovládací panel pro Master efekty (EQ, Kompresor, Limiter).
    UPDATED: Přidána hlavička souboru a textové popisky sekcí.
  ==============================================================================
*/

#include "MasterChannelStrip.h"

MasterChannelStrip::MasterChannelStrip(MasterEffects &effects) : masterEffects(effects)
{
    // Konfigurace knoflíků
    applySonarStyle(lowCutKnob, " Hz");
    lowCutKnob.setRange(20.0, 200.0, 1.0);
    lowCutKnob.onValueChange = [this]
    { masterEffects.setLowCutFrequency((float)lowCutKnob.getValue()); };

    applySonarStyle(compThresholdKnob, " dB");
    compThresholdKnob.setRange(-60.0, 0.0, 0.1);
    compThresholdKnob.onValueChange = [this]
    { masterEffects.setCompThreshold((float)compThresholdKnob.getValue()); };

    applySonarStyle(compRatioKnob, ":1");
    compRatioKnob.setRange(1.0, 20.0, 0.1);
    compRatioKnob.onValueChange = [this]
    { masterEffects.setCompRatio((float)compRatioKnob.getValue()); };

    applySonarStyle(limiterKnob, " dB");
    limiterKnob.setRange(-24.0, 0.0, 0.1);
    limiterKnob.onValueChange = [this]
    { masterEffects.setLimiterThreshold((float)limiterKnob.getValue()); };

    addAndMakeVisible(titleLabel);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::lightblue);
}

void MasterChannelStrip::applySonarStyle(juce::Slider &s, const juce::String &unit)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    s.setTextValueSuffix(unit);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff333333));
    s.setColour(juce::Slider::thumbColourId, juce::Colours::lightblue);
    addAndMakeVisible(s);
}

void MasterChannelStrip::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff151515)); // Ultra tmavé Sonar pozadí

    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.fillRoundedRectangle(getLocalBounds().reduced(5).toFloat(), 10.0f);

    // --- POPISKY SEKCI (Labels) ---
    g.setColour(juce::Colours::grey);
    g.setFont(juce::Font(12.0f, juce::Font::bold));

    auto area = getLocalBounds().reduced(15);
    area.removeFromTop(30); // Prostor pod hlavním titulkem

    int colW = area.getWidth() / 3;

    // Vykreslení textu nad jednotlivé sloupce knoflíků
    g.drawText("LOW CUT", area.getX(), area.getY(), colW, 20, juce::Justification::centred);
    g.drawText("COMPRESSOR", area.getX() + colW, area.getY(), colW, 20, juce::Justification::centred);
    g.drawText("LIMITER", area.getX() + 2 * colW, area.getY(), colW, 20, juce::Justification::centred);
}

void MasterChannelStrip::resized()
{
    auto area = getLocalBounds().reduced(15);
    titleLabel.setBounds(area.removeFromTop(30));

    auto columnWidth = area.getWidth() / 3;

    // Rozložení do sekcí
    auto eqArea = area.removeFromLeft(columnWidth);
    lowCutKnob.setBounds(eqArea.reduced(10));

    auto compArea = area.removeFromLeft(columnWidth);
    compThresholdKnob.setBounds(compArea.removeFromTop(compArea.getHeight() / 2).reduced(10));
    compRatioKnob.setBounds(compArea.reduced(10));

    auto limArea = area;
    limiterKnob.setBounds(limArea.reduced(10));
}