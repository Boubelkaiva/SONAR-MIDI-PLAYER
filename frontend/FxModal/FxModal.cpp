/*
  ==============================================================================
    FILE: FxModal.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation with rounded corners (radius: 8.0f).
  ==============================================================================
*/

#include "FxModal.h"

FxModal::FxModal(int trackNumber, Listener l)
    : trackNum(trackNumber), listener(l)
{
    // Velikost okna pro 3 řádky
    setSize(360, 220);

    // --- TITULEK ---
    addAndMakeVisible(title);
    title.setText("TRACK " + juce::String(trackNum) + " - FX SETTINGS", juce::dontSendNotification);
    title.setFont(juce::Font(15.0f, juce::Font::bold));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfffca503)); // Oranžová

    // --- CLOSE BUTTON ---
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("X");
    // Zprůhledníme tlačítko a uděláme ho šedé, aby nerušilo
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
    closeButton.onClick = [this]
    { if (listener.onClose) listener.onClose(); };

    // --- SLIDERY SETUP (0 uprostřed) ---
    auto setupBipolar = [](juce::Slider &s)
    {
        s.setRange(-64, 64, 1);
        s.setValue(0);
    };

    setupSlider(panSlider, panLabel, "PAN");
    setupBipolar(panSlider);
    panSlider.textFromValueFunction = [](double v)
    {
        if (v == 0)
            return juce::String("0");
        return (v > 0 ? "L" : "R") + juce::String(std::abs((int)v));
    };

    setupSlider(reverbSlider, reverbLabel, "REVERB");
    setupBipolar(reverbSlider);
    reverbSlider.textFromValueFunction = [](double v)
    { return juce::String((int)v); };

    setupSlider(chorusSlider, chorusLabel, "CHORUS");
    setupBipolar(chorusSlider);
    chorusSlider.textFromValueFunction = [](double v)
    { return juce::String((int)v); };

    // --- CALLBACKS ---
    panSlider.onValueChange = [this]
    { if (listener.onPanChanged) listener.onPanChanged((int)panSlider.getValue()); };
    reverbSlider.onValueChange = [this]
    { if (listener.onReverbChanged) listener.onReverbChanged((int)reverbSlider.getValue()); };
    chorusSlider.onValueChange = [this]
    { if (listener.onChorusChanged) listener.onChorusChanged((int)chorusSlider.getValue()); };
}

FxModal::~FxModal() {}

void FxModal::setupSlider(juce::Slider &s, juce::Label &l, const juce::String &name)
{
    addAndMakeVisible(l);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(12.0f, juce::Font::bold));
    l.setJustificationType(juce::Justification::centredRight);
    l.setColour(juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible(s);
    s.setSliderStyle(juce::Slider::LinearHorizontal);
    s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);

    // Styl slideru podle Audio Hardware Settings
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xfffca503));
    s.setColour(juce::Slider::trackColourId, juce::Colours::black.withAlpha(0.3f));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white.withAlpha(0.1f));
}

void FxModal::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    const float radius = 8.0f; // Tady nastavujeme radius

    // --- POZADÍ (Zaoblené) ---
    g.setColour(juce::Colour(0xff2b2b2b)); // Tmavě šedá
    g.fillRoundedRectangle(bounds, radius);

    // --- HORNÍ "LIŠTA" (Zaoblená nahoře) ---
    // Musíme vykreslit černý obdélník a pak ho oříznout zaoblením
    g.setColour(juce::Colours::black.withAlpha(0.3f));

    // Oprava: addRoundedRectangle bere Rectangle<float>, cornerSize, a volitelně bools pro rohy
    juce::Path headerPath;
    headerPath.addRoundedRectangle(0.0f, 0.0f, (float)getWidth(), 35.0f,
                                   radius, radius,
                                   true, true, false, false);

    g.fillPath(headerPath);

    // --- ORANŽOVÝ OBŘYS (Zaoblený) ---
    g.setColour(juce::Colour(0xfffca503).withAlpha(0.6f)); // Oranžová s průhledností
    g.drawRoundedRectangle(bounds, radius, 1.2f);          // Tloušťka čáry 1.2px
}

void FxModal::resized()
{
    auto r = getLocalBounds();

    // Titulek
    auto header = r.removeFromTop(35).reduced(15, 0);
    title.setBounds(header.removeFromLeft(200));
    closeButton.setBounds(getWidth() - 30, 7, 20, 20);

    r.reduce(15, 10);

    // Layout řádků (každý má 50px výšku)
    auto layoutRow = [](juce::Label &l, juce::Slider &s, juce::Rectangle<int> area)
    {
        l.setBounds(area.removeFromLeft(80).withSizeKeepingCentre(80, 20));
        s.setBounds(area.reduced(0, 10));
    };

    layoutRow(panLabel, panSlider, r.removeFromTop(50));
    layoutRow(reverbLabel, reverbSlider, r.removeFromTop(50));
    layoutRow(chorusLabel, chorusSlider, r.removeFromTop(50));
}