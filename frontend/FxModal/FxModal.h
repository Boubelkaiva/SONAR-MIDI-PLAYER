/*
  ==============================================================================
    FILE: FxModal.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Clean header with rounded corners declaration.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FxModal : public juce::Component
{
public:
    struct Listener
    {
        std::function<void(int)> onPanChanged;
        std::function<void(int)> onReverbChanged;
        std::function<void(int)> onChorusChanged;
        std::function<void()> onClose;
    };

    FxModal(int trackNumber, Listener listener);
    ~FxModal() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    int trackNum;
    Listener listener;

    juce::Slider panSlider;
    juce::Slider reverbSlider;
    juce::Slider chorusSlider;

    juce::Label panLabel, reverbLabel, chorusLabel;
    juce::Label title;
    juce::TextButton closeButton;

    void setupSlider(juce::Slider &s, juce::Label &l, const juce::String &name);

    // Standardní JUCE detektor úniků paměti
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxModal)
};