/*
  ==============================================================================
    FILE: Icons.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Generátor ikon pro aplikaci. Obsahuje neonově zelenou šipku.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Icons
{
public:
    static juce::Image getSonarIcon()
    {
        // Vytvoříme čtvercový obrázek 32x32 (standard pro ikony oken)
        juce::Image icon(juce::Image::ARGB, 32, 32, true);
        juce::Graphics g(icon);

        // Barva: Ta naše neonově zelená
        auto neonGreen = juce::Colour(0xff39ff14);
        auto bounds = juce::Rectangle<float>(4, 4, 24, 24);

        // Volitelné: Jemná záře na pozadí šipky
        g.setColour(neonGreen.withAlpha(0.2f));
        juce::Path glowPath;
        glowPath.addTriangle(bounds.getX() - 1, bounds.getY() - 1,
                             bounds.getX() - 1, bounds.getBottom() + 1,
                             bounds.getRight() + 1, bounds.getCentreY());
        g.fillPath(glowPath);

        // Hlavní šipka (Play symbol)
        g.setColour(neonGreen);
        juce::Path p;
        p.addTriangle(bounds.getX(), bounds.getY(),
                      bounds.getX(), bounds.getBottom(),
                      bounds.getRight(), bounds.getCentreY());

        g.fillPath(p);

        return icon;
    }
};