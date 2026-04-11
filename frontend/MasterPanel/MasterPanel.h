/*
  ==============================================================================

    FILE: MasterPanel.h
    NAME: MasterPanel.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Header for the master control panel.
                 UPDATED: Added currentMidiLabel for visual feedback.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MasterPanel : public juce::Component
{
public:
  MasterPanel();
  ~MasterPanel() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  // --- Callbacks ---

  // AI: Callback pro změnu hlasitosti (0-127)
  std::function<void(float volume)> onVolumeChanged;

  // AI: Callback pro výběr MIDI/KAR souboru
  std::function<void(const juce::File &)> onMidiFileSelected;

private:
  juce::Slider masterVolume;
  juce::TextButton chooseFolderButton{u8"Select Folder"};
  juce::TextButton loadMidiButton{u8"Load MIDI/KAR"};
  juce::ToggleButton headphonesButton{u8"Headphones Off"};

  // AI: Popisek, který vypíše název nahrané MIDI vedle sluchátek
  juce::Label currentMidiLabel;

  // AI: Drží instanci výběrového okna
  std::unique_ptr<juce::FileChooser> midiChooser;

  // AI: Stav pro přepínání sluchátek
  bool headphonesOn{false};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterPanel)
};