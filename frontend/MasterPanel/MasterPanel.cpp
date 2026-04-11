/*
  ==============================================================================
    FILE: MasterPanel.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation of the master control panel.
                 ADDED: Visual feedback for loaded MIDI file.
  ==============================================================================
*/

#include "MasterPanel.h"

MasterPanel::MasterPanel()
{
  // --- Master volume slider ---
  addAndMakeVisible(masterVolume);
  masterVolume.setRange(0.0, 127.0, 1.0);
  masterVolume.setValue(100.0);
  masterVolume.setSliderStyle(juce::Slider::LinearHorizontal);
  masterVolume.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);

  masterVolume.onValueChange = [this]()
  {
    if (onVolumeChanged)
      onVolumeChanged((float)masterVolume.getValue());
  };

  // --- Buttons & Labels ---
  addAndMakeVisible(chooseFolderButton);
  addAndMakeVisible(loadMidiButton);
  addAndMakeVisible(headphonesButton);

  // AI: Nastavení popisku pro název souboru
  addAndMakeVisible(currentMidiLabel);
  currentMidiLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
  currentMidiLabel.setFont(juce::Font(14.0f, juce::Font::bold));

  // --- Load MIDI/KAR button ---
  loadMidiButton.onClick = [this]()
  {
    midiChooser = std::make_unique<juce::FileChooser>(
        u8"Vyber MIDI nebo KAR soubor...",
        juce::File{},
        "*.mid;*.kar");

    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    midiChooser->launchAsync(flags, [this](const juce::FileChooser &fc)
                             {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                // AI: Aktualizace textu v panelu
                currentMidiLabel.setText(u8"Loaded: " + file.getFileName(), juce::dontSendNotification);
                
                if (onMidiFileSelected)
                    onMidiFileSelected(file);
            } });
  };

  // --- Headphones toggle ---
  headphonesButton.onClick = [this]()
  {
    headphonesOn = !headphonesOn;
    headphonesButton.setButtonText(headphonesOn ? u8"Headphones On" : u8"Headphones Off");
  };
}

MasterPanel::~MasterPanel() {}

void MasterPanel::paint(juce::Graphics &g)
{
  g.fillAll(juce::Colour(0xff151515));
  g.setColour(juce::Colours::grey.withAlpha(0.2f));
  g.drawRect(getLocalBounds(), 1);
}

void MasterPanel::resized()
{
  auto r = getLocalBounds().reduced(10);

  // --- Slider nahoře ---
  int sliderHeight = 30;
  masterVolume.setBounds(r.removeFromTop(sliderHeight));
  r.removeFromTop(5);

  // --- Buttons & Label v jedné řadě ---
  int buttonWidth = 110;

  chooseFolderButton.setBounds(r.removeFromLeft(buttonWidth).reduced(0, 5));
  r.removeFromLeft(10);

  loadMidiButton.setBounds(r.removeFromLeft(buttonWidth).reduced(0, 5));
  r.removeFromLeft(10);

  headphonesButton.setBounds(r.removeFromLeft(buttonWidth).reduced(0, 5));
  r.removeFromLeft(15); // Trochu větší mezera před textem

  // AI: Zbytek místa vpravo využijeme pro název souboru
  currentMidiLabel.setBounds(r.reduced(0, 5));
}