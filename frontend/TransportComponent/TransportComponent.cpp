/*
  ==============================================================================
    FILE: TransportComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Transport controls + Audio Settings Button.
    FIXED: Added explicit console logging for event tracking.
  ==============================================================================
*/

#include "TransportComponent.h"
#include "../../backend/MidiPlayer/MidiPlayer.h"
#include "../../backend/AudioSettingsPanel/AudioSettingsPanel.h"
#include <iostream>

TransportComponent::TransportComponent(MidiPlayer &player) : midiPlayer(player)
{
    addAndMakeVisible(startButton);
    startButton.setButtonText("Start");

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");

    addAndMakeVisible(pauseButton);
    pauseButton.setButtonText("Pause");

    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Audio HW");
    settingsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);

    // --- KLÍČOVÁ LOGIKA S DIAGNOSTIKOU ---

    startButton.onClick = [this]
    {
        std::cout << "[UI] Transport: Kliknuto na START" << std::endl;
        if (onStartClicked)
        {
            onStartClicked();
        }
        else
        {
            std::cout << "[CRITICAL] Transport: onStartClicked neni propojen v MainComponent!" << std::endl;
        }
    };

    stopButton.onClick = [this]
    {
        std::cout << "[UI] Transport: Kliknuto na STOP" << std::endl;
        if (onStopClicked)
            onStopClicked();
    };

    pauseButton.onClick = [this]
    {
        std::cout << "[UI] Transport: Kliknuto na PAUSE" << std::endl;
        if (onPauseClicked)
            onPauseClicked();
    };

    settingsButton.onClick = [this]
    {
        std::cout << "[UI] Oteviram nastaveni zvuku..." << std::endl;

        auto *settingsPanel = new AudioSettingsPanel(midiPlayer.getDeviceManager());
        settingsPanel->setSize(400, 300);

        juce::DialogWindow::LaunchOptions dialogOptions;
        dialogOptions.content.setOwned(settingsPanel);
        dialogOptions.dialogTitle = "Audio Hardware Settings";
        dialogOptions.dialogBackgroundColour = juce::Colour(0xff2b2b2b);
        dialogOptions.escapeKeyTriggersCloseButton = true;
        dialogOptions.useNativeTitleBar = true;
        dialogOptions.resizable = false;

        if (auto *dw = dialogOptions.launchAsync())
        {
            dw->setIcon(juce::Image()); // Odstraní výchozí JUCE ikonu z dialogu
        }
    };
}

TransportComponent::~TransportComponent() {}

void TransportComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff202020));
}

void TransportComponent::resized()
{
    auto area = getLocalBounds();
    int buttonWidth = area.getWidth() / 4;

    startButton.setBounds(area.removeFromLeft(buttonWidth).reduced(2));
    stopButton.setBounds(area.removeFromLeft(buttonWidth).reduced(2));
    pauseButton.setBounds(area.removeFromLeft(buttonWidth).reduced(2));
    settingsButton.setBounds(area.reduced(2));
}