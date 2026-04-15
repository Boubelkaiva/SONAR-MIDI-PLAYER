/*
  ==============================================================================
    FILE: VUMeter.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementace vykreslování a animace VU metru.
  ==============================================================================
*/

#include "VUMeter.h"

VUMeter::VUMeter()
{
    startTimerHz(30); // 30 FPS pro plynulý pohyb
}

VUMeter::~VUMeter()
{
    stopTimer();
}

void VUMeter::trigger(int velocity)
{
    // Normalizace velocity na rozsah 0.0 - 1.0
    float newLevel = (float)velocity / 127.0f;

    // Pokud je nová nota silnější než aktuální stav, hned tam skočíme
    if (newLevel > currentLevel)
        currentLevel = newLevel;

    repaint();
}

void VUMeter::timerCallback()
{
    if (currentLevel > 0.0f)
    {
        currentLevel *= decayFactor;

        if (currentLevel < 0.005f)
            currentLevel = 0.0f;

        repaint();
    }
}

void VUMeter::paint(juce::Graphics &g)
{
    auto area = getLocalBounds().toFloat();

    // Pozadí (šachta metru) - tmavší než hlavní pozadí 0xff151515
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillRect(area);

    if (currentLevel > 0.0f)
    {
        auto fillHeight = area.getHeight() * currentLevel;
        auto fillRect = area.withTop(area.getBottom() - fillHeight);

        // Gradient v barvách Sonaru
        juce::ColourGradient gradient(juce::Colours::green, 0, area.getBottom(),
                                      juce::Colours::red, 0, area.getY(), false);

        // Přidáme žlutou do středu pro lepší přechod
        gradient.addColour(0.6, juce::Colours::yellow);

        g.setGradientFill(gradient);
        g.fillRect(fillRect);
    }

    // Jemný rámeček pro ohraničení
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRect(area, 1.0f);
}