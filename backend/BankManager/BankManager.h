/*
==============================================================================

  FILE: BankManager.h
  PROJECT: SONAR MIDI PLAYER
  MODULE: backend/BankManager
  DESCRIPTION: Handles SF2 bank list management, BankDNA pipeline,
               instrument mapping and FINAL UI model generation.
               Produces frontend-ready snapshot (InstrumentModal input).

==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <functional>

// ======================================================
// BANKMANAGER - DATA PIPELINE ENGINE
// ======================================================

class BankManager
{
public:
  // ======================================================
  // LIFECYCLE
  // ======================================================

  BankManager();
  ~BankManager() = default;

  // ======================================================
  // BANK REGISTRY API
  // ======================================================

  void addBank(const juce::File &file);
  void removeBank(int index);

  const std::vector<juce::File> &getLoadedBanks() const { return loadedBanks; }

  // ======================================================
  // PERSISTENCE
  // ======================================================

  void saveConfig();
  void loadConfig();

  static juce::File getSettingsFile();

  // ======================================================
  // PIPELINE
  // ======================================================

  void buildRuntimePipeline();

  void loadBankDNA();
  void loadInstrumentMap();
  void loadTemplates();
  void resolveCategories();
  void buildUIModel();

  // ======================================================
  // RUNTIME
  // ======================================================

  void reloadAllBanksAndRebuildAndPush();
  void pushModelToFrontend();

  // 🔥 DOPLNĚNO (CHYBĚLO V .H)
  void exportFinalJSON();

  // ======================================================
  // FRONTEND CALLBACK
  // ======================================================

  std::function<void(const juce::var &)> onFrontendModelReady;

  // ======================================================
  // STATE
  // ======================================================

  bool isReady() const { return pipelineReady; }

  juce::var getUIModelSnapshot() const { return uiModel; }

private:
  // RAW DATA
  std::vector<juce::File> loadedBanks;

  // STATE
  bool pipelineReady = false;

  // 🔥 FIX: MUSÍ EXISTOVAT (jinak cpp padá / IntelliSense hlásí chybu)
  juce::var uiModel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BankManager)
};