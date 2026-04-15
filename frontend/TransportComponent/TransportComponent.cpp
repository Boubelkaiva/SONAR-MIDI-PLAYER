/*
  ==============================================================================
    FILE: TransportComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Hlavní kontejner, který skládá komponenty pod sebe.
                 Barvy SettingsBar jsou definovány v SettingsBarComponent.cpp.
  ==============================================================================
*/

#include "TransportComponent.h"

TransportComponent::TransportComponent(MidiPlayer &player, BankManager &manager)
    : midiPlayer(player)
{
    // 1. TRANSPORT TLAČÍTKA (Tato zůstávají zde, protože jsou součástí TransportComponent)
    addAndMakeVisible(startButton);
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2d5a27));

    addAndMakeVisible(stopButton);
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff7a1a1a));

    addAndMakeVisible(pauseButton);

    // 2. SETTINGS BAR (Vlepení samostatné komponenty - barvy má v sobě)
    settingsBar = std::make_unique<SettingsBarComponent>(midiPlayer);
    addAndMakeVisible(settingsBar.get());

    // 3. SF2 LIST
    sf2List = std::make_unique<SF2ListComponent>(manager);
    addAndMakeVisible(sf2List.get());

    // 4. VERSION FOOTER
    versionFooter = std::make_unique<VersionFooter>();
    addAndMakeVisible(versionFooter.get());

    // Callbacky pro transport
    startButton.onClick = [this]
    { if (onStartClicked) onStartClicked(); };
    stopButton.onClick = [this]
    { if (onStopClicked) onStopClicked(); };
    pauseButton.onClick = [this]
    { if (onPauseClicked) onPauseClicked(); };
}

TransportComponent::~TransportComponent() {}

void TransportComponent::paint(juce::Graphics &g)
{
    // Hlavní pozadí celého panelu
    g.fillAll(juce::Colour(0xff151515));

    g.setColour(juce::Colours::white.withAlpha(0.2f));

    // Dělící čáry mezi logickými bloky
    if (startButton.isVisible())
        g.drawHorizontalLine(startButton.getBottom() + 6, 10.0f, (float)getWidth() - 10.0f);

    if (settingsBar)
        g.drawHorizontalLine(settingsBar->getBottom() + 6, 10.0f, (float)getWidth() - 10.0f);

    if (versionFooter)
        g.drawHorizontalLine(versionFooter->getY() - 6, 10.0f, (float)getWidth() - 10.0f);
}

void TransportComponent::resized()
{
    auto area = getLocalBounds().reduced(8);

    const int transportHeight = 45;
    const int settingsHeight = 45;
    const int footerHeight = 25;
    const int spacing = 12;

    // A. Verze úplně dospod
    if (versionFooter)
        versionFooter->setBounds(area.removeFromBottom(footerHeight));

    area.removeFromBottom(spacing);

    // B. Horní blok (Transport + Settings)
    auto topArea = area.removeFromTop(transportHeight + spacing + settingsHeight);

    // Transport row
    auto transportRow = topArea.removeFromTop(transportHeight);
    int btnW = transportRow.getWidth() / 3;
    startButton.setBounds(transportRow.removeFromLeft(btnW).reduced(2));
    stopButton.setBounds(transportRow.removeFromLeft(btnW).reduced(2));
    pauseButton.setBounds(transportRow.reduced(2));

    topArea.removeFromTop(spacing);

    // Settings row (Sem se vlepí ta tvoje barevná komponenta)
    if (settingsBar)
        settingsBar->setBounds(topArea.removeFromTop(settingsHeight));

    area.removeFromTop(spacing);

    // C. Banky (Zbytek místa uprostřed)
    if (sf2List)
        sf2List->setBounds(area);
}