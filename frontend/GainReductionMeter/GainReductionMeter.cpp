/*
  ==============================================================================
    FILE: GainReductionMeter.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementace vizualizace útlumu (shora dolů) v Sonar stylu.
    UPDATED: Oprava propojení třídy a přidání hlavičky.
  ==============================================================================
*/

#include "GainReductionMeter.h"

GainReductionMeter::GainReductionMeter()
{
    startTimerHz(30);
}

GainReductionMeter::~GainReductionMeter()
{
    stopTimer();
}

void GainReductionMeter::setReduction(float reduction)
{
    if (reduction > currentReduction)
        currentReduction = reduction;

    repaint();
}

void GainReductionMeter::timerCallback()
{
    if (currentReduction > 0.0f)
    {
        currentReduction -= fallSpeed * 0.1f;
        if (currentReduction < 0.0f)
            currentReduction = 0.0f;
        repaint();
    }
}

void GainReductionMeter::paint(juce::Graphics &g)
{
    auto area = getLocalBounds().toFloat();

    // Pozadí
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillRect(area);

    if (currentReduction > 0.01f)
    {
        // Gain Reduction se kreslí shora dolů (oranžová barva)
        auto barHeight = area.getHeight() * currentReduction;
        auto barArea = area.removeFromTop(barHeight);

        g.setColour(juce::Colour(0xfffca503));
        g.fillRect(barArea);
    }

    // Okraj
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRect(area, 1.0f);
}
