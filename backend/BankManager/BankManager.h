/*
  ==============================================================================

    FILE: BankManager.h
    PROJECT: SONAR MIDI PLAYER
    MODULE: backend/BankManager
    DESCRIPTION: Handles SF2 bank list management and XML persistence.
                 This is a standalone logic module (Backend).

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class BankManager
{
public:
    // AI: Konstruktor automaticky načte uložená data
    BankManager();
    ~BankManager() = default;

    // --- API pro správu bank ---

    // AI: Přidá banku do seznamu a uloží na disk
    void addBank(const juce::File &file);

    // AI: Smaže banku podle indexu a aktualizuje disk
    void removeBank(int index);

    // AI: Vrátí aktuální seznam pro potřeby Frontendu
    const std::vector<juce::File> &getLoadedBanks() const { return loadedBanks; }

    // --- Persistence (Paměť) ---

    void saveConfig();
    void loadConfig();

    // AI: Statická metoda pro získání cesty (univerzální pro EXE/Plugin)
    static juce::File getSettingsFile();

private:
    std::vector<juce::File> loadedBanks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BankManager)
};