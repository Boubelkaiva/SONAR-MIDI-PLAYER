/*
  ==============================================================================
    FILE: FxModal.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation with rounded corners and correct MIDI 0-127 ranges.
    UPDATED: Přidán podrobný debug výpis pro kontrolu příchozích dat z analýzy.
  ==============================================================================
*/

#include "FxModal.h"
#include <iostream>

FxModal::FxModal(int trackNumber, Listener l)
    : trackNum(trackNumber), listener(l)
{
    std::cout << "[POG] FE: Otevírám FxModal pro Track " << trackNum << std::endl;

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
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
    closeButton.onClick = [this]
    { if (listener.onClose) listener.onClose(); };

    // --- PAN SLIDER (0-127, Center 64) ---
    setupSlider(panSlider, panLabel, "PAN");
    panSlider.setRange(0, 127, 1);
    panSlider.setValue(64);
    panSlider.textFromValueFunction = [](double v)
    {
        if (v == 64)
            return juce::String("C");
        if (v < 64)
            return "L " + juce::String(64 - (int)v);
        return "R " + juce::String((int)v - 64);
    };

    // --- REVERB SLIDER (0-127) ---
    setupSlider(reverbSlider, reverbLabel, "REVERB");
    reverbSlider.setRange(0, 127, 1);
    reverbSlider.setValue(0);
    reverbSlider.textFromValueFunction = [](double v)
    { return juce::String((int)v); };

    // --- CHORUS SLIDER (0-127) ---
    setupSlider(chorusSlider, chorusLabel, "CHORUS");
    chorusSlider.setRange(0, 127, 1);
    chorusSlider.setValue(0);
    chorusSlider.textFromValueFunction = [](double v)
    { return juce::String((int)v); };

    // --- CALLBACKS ---
    panSlider.onValueChange = [this]
    {
        int val = (int)panSlider.getValue();
        std::cout << "[POG] FE: FxModal Track " << trackNum << " -> UI PAN: " << val << std::endl;
        if (listener.onPanChanged)
            listener.onPanChanged(val);
    };

    reverbSlider.onValueChange = [this]
    {
        int val = (int)reverbSlider.getValue();
        std::cout << "[POG] FE: FxModal Track " << trackNum << " -> UI REVERB: " << val << std::endl;
        if (listener.onReverbChanged)
            listener.onReverbChanged(val);
    };

    chorusSlider.onValueChange = [this]
    {
        int val = (int)chorusSlider.getValue();
        std::cout << "[POG] FE: FxModal Track " << trackNum << " -> UI CHORUS: " << val << std::endl;
        if (listener.onChorusChanged)
            listener.onChorusChanged(val);
    };
}

FxModal::~FxModal() {}

/** Nastaví počáteční hodnoty načtené z analýzy MIDI souboru */
void FxModal::setInitialValues(int pan, int reverb, int chorus)
{
    std::cout << "[POG] FE: FxModal " << trackNum << " nastavuje hodnoty z analýzy (P:" << pan << " R:" << reverb << " C:" << chorus << ")" << std::endl;

    // Používáme dontSendNotification, abychom při otevírání okna
    // nespustili zpětnou vazbu do Playeru, protože ty hodnoty už tam jsou.
    panSlider.setValue(pan, juce::dontSendNotification);
    reverbSlider.setValue(reverb, juce::dontSendNotification);
    chorusSlider.setValue(chorus, juce::dontSendNotification);
}

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

    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xfffca503));
    s.setColour(juce::Slider::trackColourId, juce::Colours::black.withAlpha(0.3f));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white.withAlpha(0.1f));
}

void FxModal::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();
    const float radius = 8.0f;

    // --- POZADÍ ---
    g.setColour(juce::Colour(0xff2b2b2b));
    g.fillRoundedRectangle(bounds, radius);

    // --- HORNÍ LIŠTA ---
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    juce::Path headerPath;
    headerPath.addRoundedRectangle(0.0f, 0.0f, (float)getWidth(), 35.0f,
                                   radius, radius,
                                   true, true, false, false);
    g.fillPath(headerPath);

    // --- ORANŽOVÝ OBŘYS ---
    g.setColour(juce::Colour(0xfffca503).withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, radius, 1.2f);
}

void FxModal::resized()
{
    auto r = getLocalBounds();

    auto header = r.removeFromTop(35).reduced(15, 0);
    title.setBounds(header.removeFromLeft(200));
    closeButton.setBounds(getWidth() - 30, 7, 20, 20);

    r.reduce(15, 10);

    auto layoutRow = [](juce::Label &l, juce::Slider &s, juce::Rectangle<int> area)
    {
        l.setBounds(area.removeFromLeft(80).withSizeKeepingCentre(80, 20));
        s.setBounds(area.reduced(0, 10));
    };

    layoutRow(panLabel, panSlider, r.removeFromTop(50));
    layoutRow(reverbLabel, reverbSlider, r.removeFromTop(50));
    layoutRow(chorusLabel, chorusSlider, r.removeFromTop(50));
}