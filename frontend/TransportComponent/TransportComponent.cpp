/*
  ==============================================================================
    FILE: TransportComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Rodič pro pravý panel - Transport, Settings, Banky, Verze.
    UPDATED: Ajka - Oprava LNK2019 a finální bootstrap layout.
  ==============================================================================
*/

#include "TransportComponent.h"

TransportComponent::TransportComponent(MidiPlayer &player, BankManager &manager)
    : midiPlayer(player)
{
    // 1. Horní transport (Tlačítka)
    addAndMakeVisible(startButton);
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2d5a27));
    addAndMakeVisible(stopButton);
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff7a1a1a));
    addAndMakeVisible(pauseButton);

    // 2. Settings Bar (Audio HW + Master FX)
    settingsBar = std::make_unique<SettingsBarComponent>(midiPlayer);
    addAndMakeVisible(settingsBar.get());

    // 3. SF2 LIST (Banky)
    sf2List = std::make_unique<SF2ListComponent>(manager);
    addAndMakeVisible(sf2List.get());

    // 4. VERSION FOOTER (Verze úplně dole)
    versionFooter = std::make_unique<VersionFooter>();
    addAndMakeVisible(versionFooter.get());

    // Callbacky pro tlačítka
    startButton.onClick = [this]
    {
        std::cout << "[UI] START BUTTON CLICKED" << std::endl;

        if (onStartClicked)
            onStartClicked();
    };
    stopButton.onClick = [this]
    { if (onStopClicked)  onStopClicked(); };
    pauseButton.onClick = [this]
    { if (onPauseClicked) onPauseClicked(); };
}

// Implementace destruktoru (Fix pro chybu LNK2019)
TransportComponent::~TransportComponent()
{
}

void TransportComponent::paint(juce::Graphics &g)
{
    // Pozadí pravého panelu
    g.fillAll(juce::Colour(0xff151515));

    g.setColour(juce::Colours::white.withAlpha(0.1f));

    // Čára pod transportními tlačítky
    g.drawHorizontalLine(startButton.getBottom() + 5, 10.0f, (float)getWidth() - 10.0f);
}

void TransportComponent::resized()
{
    auto area = getLocalBounds().reduced(8);

    const int transportHeight = 45;
    const int settingsHeight = 45;
    const int footerHeight = 25;
    const int spacing = 10;

    // A. Verze úplně dospod modulu
    if (versionFooter)
        versionFooter->setBounds(area.removeFromBottom(footerHeight));

    // Mezera nad verzí
    area.removeFromBottom(spacing);

    // B. Horní sekce - Transportní tlačítka
    auto transportRow = area.removeFromTop(transportHeight);
    int btnW = transportRow.getWidth() / 3;
    startButton.setBounds(transportRow.removeFromLeft(btnW).reduced(2));
    stopButton.setBounds(transportRow.removeFromLeft(btnW).reduced(2));
    pauseButton.setBounds(transportRow.reduced(2));

    area.removeFromTop(spacing);

    // C. Settings Bar (Audio HW a Master FX)
    if (settingsBar)
        settingsBar->setBounds(area.removeFromTop(settingsHeight));

    area.removeFromTop(spacing);

    // D. Banky (Zbytek prostoru uprostřed modulu)
    if (sf2List)
        sf2List->setBounds(area);
}