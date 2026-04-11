/*
  ==============================================================================

    FILE: TrackComponent.cpp
    NAME: TrackComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation of individual track with custom LookAndFeel.
                 FIXED: Increased track number button width to fit 2-digit numbers.

  ==============================================================================
*/

#include "TrackComponent.h"

// --- vlastní LookAndFeel pro toggle tlačítka ---
class ToggleButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics &g, juce::Button &button,
                              const juce::Colour &, bool, bool isMouseOverButton) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        juce::Colour bg;
        juce::Colour text;

        if (button.getToggleState())
        {
            if (button.getButtonText() == "M")
                bg = juce::Colours::red;
            else if (button.getButtonText() == "S")
                bg = juce::Colours::yellow;
            else
                bg = juce::Colours::green;

            text = juce::Colours::black;
        }
        else
        {
            bg = juce::Colours::grey.withAlpha(0.3f);
            text = juce::Colours::white;
        }

        g.setColour(bg);
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(isMouseOverButton ? juce::Colours::white.withAlpha(0.4f) : juce::Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        g.setColour(text);
        g.setFont(juce::Font(15.0f, juce::Font::bold));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(),
                         juce::Justification::centred, 1);
    }

    void drawButtonText(juce::Graphics &, juce::TextButton &, bool, bool) override {}
};

static ToggleButtonLookAndFeel toggleButtonLF;

TrackComponent::TrackComponent(int trackNumber, const juce::String &instrumentName, InstrumentType::Type type)
    : trackNum(trackNumber), trackName(instrumentName), instrumentType(type)
{
    // --- číslo tracku (zvětšeno a vypnuto pohlcování kliknutí) ---
    addAndMakeVisible(trackNumberButton);
    trackNumberButton.setButtonText(juce::String(trackNum));
    trackNumberButton.setEnabled(false);
    trackNumberButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.3f));
    trackNumberButton.setColour(juce::TextButton::textColourOffId, juce::Colours::orange);

    // --- název instrumentu ---
    addAndMakeVisible(nameLabel);
    nameLabel.setText(trackName, juce::dontSendNotification);
    nameLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0xff333333));
    nameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    nameLabel.setEditable(true, true, false);
    nameLabel.setJustificationType(juce::Justification::centredLeft);

    // --- volume slider ---
    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0, 127, 1);
    volumeSlider.setValue(100);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 45, 18);

    // --- Mute Button ---
    addAndMakeVisible(muteButton);
    muteButton.setButtonText("M");
    muteButton.setClickingTogglesState(true);
    muteButton.setLookAndFeel(&toggleButtonLF);

    // --- Solo Button ---
    addAndMakeVisible(soloButton);
    soloButton.setButtonText("S");
    soloButton.setClickingTogglesState(true);
    soloButton.setLookAndFeel(&toggleButtonLF);

    // --- Third Button (Vol) ---
    addAndMakeVisible(thirdButton);
    thirdButton.setButtonText("Vol");
    thirdButton.setClickingTogglesState(true);
    thirdButton.setLookAndFeel(&toggleButtonLF);
}

TrackComponent::~TrackComponent()
{
    muteButton.setLookAndFeel(nullptr);
    soloButton.setLookAndFeel(nullptr);
    thirdButton.setLookAndFeel(nullptr);
}

void TrackComponent::updateVolume(float newVolume)
{
    volumeSlider.setValue((double)newVolume, juce::dontSendNotification);
}

void TrackComponent::setInstrument(const juce::String &name, juce::Colour colour)
{
    trackName = name;
    nameLabel.setText(trackName, juce::dontSendNotification);
    nameLabel.setColour(juce::Label::backgroundColourId, colour);
}

void TrackComponent::setIcons(const juce::String &mute, const juce::String &solo, const juce::String &third)
{
    muteButton.setButtonText(mute);
    soloButton.setButtonText(solo);
    thirdButton.setButtonText(third);
}

void TrackComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff222222));

    // Spodní linka pro oddělení řádků
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());
}

void TrackComponent::resized()
{
    auto r = getLocalBounds().reduced(2);

    // AI: Zvětšeno z 30 na 45 pixelů, aby se vešla čísla 10-16
    trackNumberButton.setBounds(r.removeFromLeft(45).reduced(1));

    // AI: Zvětšeno ze 120 na 135 pro delší názvy nástrojů
    nameLabel.setBounds(r.removeFromLeft(135).reduced(1));

    int buttonWidth = 35;
    int buttonSpacing = 2;

    // Pravá strana: tlačítka
    thirdButton.setBounds(r.removeFromRight(buttonWidth).reduced(1));
    r.removeFromRight(buttonSpacing);
    soloButton.setBounds(r.removeFromRight(buttonWidth).reduced(1));
    r.removeFromRight(buttonSpacing);
    muteButton.setBounds(r.removeFromRight(buttonWidth).reduced(1));
    r.removeFromRight(buttonSpacing);

    // Slider vyplní zbývající prostor uprostřed
    volumeSlider.setBounds(r.reduced(2));
}