/*
  ==============================================================================
    FILE: MasterPanel.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Implementation of the master control panel.
    UPDATED: Robustní asynchronní načítání a vizuální zpětná vazba.
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

  // Nastavení popisku pro název souboru
  addAndMakeVisible(currentMidiLabel);
  currentMidiLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
  currentMidiLabel.setFont(juce::Font(14.0f, juce::Font::bold));
  currentMidiLabel.setText(u8"No file loaded", juce::dontSendNotification);

  // --- Load MIDI/KAR button ---
  loadMidiButton.onClick = [this]()
  {
    // Vytvoření chooseru jako unique_ptr (v MasterPanel.h musí být std::unique_ptr<juce::FileChooser> midiChooser)
    midiChooser = std::make_unique<juce::FileChooser>(
        u8"Vyber MIDI nebo KAR soubor...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.mid;*.kar");

    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    midiChooser->launchAsync(flags, [this](const juce::FileChooser &fc)
                             {
            auto file = fc.getResult();
            
            if (file != juce::File() && file.existsAsFile())
            {
                // Okamžitá vizuální odezva na UI vlákně
                currentMidiLabel.setText(u8"Loading: " + file.getFileName(), juce::dontSendNotification);
                
                // Vyvolání callbacku - MidiPlayer si to nyní převezme ve vlastním vlákně
                if (onMidiFileSelected)
                {
                    onMidiFileSelected(file);
                }
            } });
  };

  // --- Headphones toggle ---
  headphonesButton.onClick = [this]()
  {
    headphonesOn = !headphonesOn;
    headphonesButton.setButtonText(headphonesOn ? u8"Headphones On" : u8"Headphones Off");
    // Zde můžeš přidat onHeadphonesChanged(headphonesOn), pokud máš definováno
  };
}

MasterPanel::~MasterPanel()
{
  // Chooser se zničí automaticky díky unique_ptr
}

void MasterPanel::paint(juce::Graphics &g)
{
  g.fillAll(juce::Colour(0xff151515));
  g.setColour(juce::Colours::grey.withAlpha(0.2f));
  g.drawRect(getLocalBounds(), 1);
}

void MasterPanel::resized()
{
  auto r = getLocalBounds().reduced(10);

  // Slider nahoře na celou šířku
  int sliderHeight = 30;
  masterVolume.setBounds(r.removeFromTop(sliderHeight));
  r.removeFromTop(5);

  // Rozložení prvků v řadě
  int buttonWidth = 120;
  int spacing = 10;

  chooseFolderButton.setBounds(r.removeFromLeft(buttonWidth).reduced(0, 5));
  r.removeFromLeft(spacing);

  loadMidiButton.setBounds(r.removeFromLeft(buttonWidth).reduced(0, 5));
  r.removeFromLeft(spacing);

  headphonesButton.setBounds(r.removeFromLeft(buttonWidth).reduced(0, 5));
  r.removeFromLeft(spacing + 5);

  // Label zabere zbytek místa
  currentMidiLabel.setBounds(r.reduced(0, 5));
}