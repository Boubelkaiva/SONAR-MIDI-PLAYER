/*
  ==============================================================================
    FILE: SettingsBarComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementace tlačítek Master FX a Audio HW s novými barvami.
    COLORS: FX: SteelBlue (0xff4682b4), Audio: Firebrick (0xffb22222)
  ==============================================================================
*/

#include "SettingsBarComponent.h"
#include "../MasterChannelStrip/MasterChannelStrip.h"
#include "../../backend/AudioSettingsPanel/AudioSettingsPanel.h"

SettingsBarComponent::SettingsBarComponent(MidiPlayer &player) : midiPlayer(player)
{
    // --- Tlačítko Master FX (Tlumená modrá) ---
    addAndMakeVisible(fxButton);
    fxButton.setButtonText("Master FX");
    fxButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4682b4));

    fxButton.onClick = [this]
    {
        auto *fxPanel = new MasterChannelStrip(midiPlayer.getMasterEffects());
        fxPanel->setSize(500, 350);

        juce::DialogWindow::LaunchOptions opt;
        opt.content.setOwned(fxPanel);
        opt.dialogTitle = "AI MASTER PROCESSOR";
        opt.dialogBackgroundColour = juce::Colour(0xff151515); // Tmavé pozadí okna
        opt.useNativeTitleBar = true;
        opt.launchAsync();
    };

    // --- Tlačítko Audio HW (Cihlově červená) ---
    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Audio HW");
    settingsButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff741010));

    settingsButton.onClick = [this]
    {
        auto *settingsPanel = new AudioSettingsPanel(midiPlayer.getDeviceManager());
        settingsPanel->setSize(450, 350);

        juce::DialogWindow::LaunchOptions opt;
        opt.content.setOwned(settingsPanel);
        opt.dialogTitle = "AUDIO HARDWARE";
        opt.dialogBackgroundColour = juce::Colour(0xff151515); // Tmavé pozadí okna
        opt.useNativeTitleBar = true;
        opt.launchAsync();
    };
}

void SettingsBarComponent::paint(juce::Graphics &g)
{
    // Pozadí je řešeno v TransportComponent (0xff151515)
}

void SettingsBarComponent::resized()
{
    auto area = getLocalBounds();
    int btnWidth = area.getWidth() / 2;

    // Tlačítka vedle sebe s mírným zmenšením pro mezery
    fxButton.setBounds(area.removeFromLeft(btnWidth).reduced(2));
    settingsButton.setBounds(area.reduced(2));
}