/*
  ==============================================================================

    FILE: SF2ListComponent.h
    NAME: SF2ListComponent.h
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Header for the SF2 bank list.
                 FIXED: Connected to BankManager (Backend) and removed local storage.
                 RESTORED: Callback 'onSelectionChanged' for MainComponent.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../backend/BankManager/BankManager.h"

class SF2ListComponent : public juce::Component,
                         public juce::FileDragAndDropTarget,
                         public juce::TableListBoxModel
{
public:
  // AI: Konstruktor nyní přijímá referenci na BankManager z backendu
  SF2ListComponent(BankManager &manager);
  ~SF2ListComponent() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  // --- TableListBoxModel metody (nyní berou data z bankManageru) ---
  int getNumRows() override;
  void paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected) override;
  void paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

  // --- Logika pro správu bank (volá metody v bankManageru) ---
  void loadSF2(const juce::File &file);
  void deleteSelectedBank();

  // AI: Callback pro MainComponent (výběr banky pro přehrávání)
  std::function<void(const juce::File &selectedFile)> onSelectionChanged;

  // Drag & Drop
  bool isInterestedInFileDrag(const juce::StringArray &files) override;
  void filesDropped(const juce::StringArray &files, int x, int y) override;

private:
  // AI: Odkaz na centrální správu bank
  BankManager &bankManager;

  juce::TableListBox sf2List{"SF2 Table"};

  juce::TextButton chooseFolderButton;
  juce::TextButton deleteBankButton;

  std::unique_ptr<juce::FileChooser> fc;
  juce::File lastBankDirectory;

  // AI: 'loadedBanks' zde již není, používáme bankManager.getLoadedBanks()

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SF2ListComponent)
};