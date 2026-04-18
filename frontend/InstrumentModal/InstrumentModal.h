/*
 ==============================================================================
   FILE: InstrumentModal.h
   PROJECT: SONAR MIDI PLAYER
   DESCRIPTION: Třísloupcový výběr nástroje (Bank, Category, Instrument)
   UPDATED: Fixed JSON loading + stable AppData mapping + BE model binding
 ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <functional>

// ======================================================
// INSTRUMENT MODAL (UI + BE DATA BINDING HOOK)
// ======================================================

class InstrumentModal : public juce::Component
{
public:
  // ======================================================
  // LIFECYCLE
  // ======================================================
  InstrumentModal(int trackIdx,
                  std::function<void(int, int, int)> onSelected);

  ~InstrumentModal() override;

  // ======================================================
  // UI
  // ======================================================
  void paint(juce::Graphics &g) override;
  void resized() override;

  // ======================================================
  // 🔥 DATA BINDING (BANKMANAGER → UI)
  // ======================================================
  void setModel(const juce::var &model);

private:
  // ======================================================
  // INTERNAL LOADERS
  // ======================================================
  void loadBankData();

private:
  // ======================================================
  // TRACK CONTEXT
  // ======================================================
  int trackIndex;

  // ======================================================
  // CALLBACK (UI → BE selection output)
  // ======================================================
  std::function<void(int, int, int)> onInstrumentSelected;

  // ======================================================
  // UI COMPONENTS
  // ======================================================
  juce::Label bankLabel, categoryLabel, instrumentLabel;

  juce::ListBox bankListBox;
  juce::ListBox categoryListBox;
  juce::ListBox instrumentListBox;

  // ======================================================
  // RAW DATA STORAGE (future parsing layer)
  // ======================================================
  juce::Array<juce::var> instruments;

  juce::File currentJsonFile;

  // ======================================================
  // JUCE SAFETY
  // ======================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentModal)
};