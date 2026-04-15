#pragma once
#include <JuceHeader.h>

class VersionFooter : public juce::Component
{
public:
    VersionFooter()
    {
        addAndMakeVisible(versionLabel);
        versionLabel.setText("v" + juce::String(ProjectInfo::versionString), juce::dontSendNotification);
        versionLabel.setFont(juce::Font(11.0f, juce::Font::bold));
        versionLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.6f));
        versionLabel.setJustificationType(juce::Justification::centred);
    }

    void paint(juce::Graphics &g) override
    {
        // Volitelná linka nad verzí, aby se oddělila od bank
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.drawHorizontalLine(1, 20.0f, (float)getWidth() - 20.0f);
    }

    void resized() override
    {
        versionLabel.setBounds(getLocalBounds());
    }

private:
    juce::Label versionLabel;
};