/*
  ==============================================================================
    FILE: TransportComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Transport controls (Start, Stop, Pause).
    FIXED: Added console logging to track button clicks in VS Code.
  ==============================================================================
*/

#include "TransportComponent.h"
#include "../../backend/MidiPlayer/MidiPlayer.h"
#include <iostream> // Nutné pro logování do konzole

TransportComponent::TransportComponent(MidiPlayer &player) : midiPlayer(player)
{
    addAndMakeVisible(startButton);
    startButton.setButtonText("Start");

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");

    addAndMakeVisible(pauseButton);
    pauseButton.setButtonText("Pause");

    // --- Start ---
    startButton.onClick = [this]
    {
        std::cout << "[UI] Transport: Kliknuto na START" << std::endl;
        if (onStartClicked)
            onStartClicked();
        else
            std::cout << "[WARN] Transport: onStartClicked callback neni propojen!" << std::endl;
    };

    // --- Stop ---
    stopButton.onClick = [this]
    {
        std::cout << "[UI] Transport: Kliknuto na STOP" << std::endl;
        if (onStopClicked)
            onStopClicked();
        else
            std::cout << "[WARN] Transport: onStopClicked callback neni propojen!" << std::endl;
    };

    // --- Pause ---
    pauseButton.onClick = [this]
    {
        std::cout << "[UI] Transport: Kliknuto na PAUSE" << std::endl;
        if (onPauseClicked)
            onPauseClicked();
        else
            std::cout << "[WARN] Transport: onPauseClicked callback neni propojen!" << std::endl;
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
    int buttonWidth = area.getWidth() / 3;

    startButton.setBounds(area.removeFromLeft(buttonWidth).reduced(2));
    stopButton.setBounds(area.removeFromLeft(buttonWidth).reduced(2));
    pauseButton.setBounds(area.reduced(2));
}