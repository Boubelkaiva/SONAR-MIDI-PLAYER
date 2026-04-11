/*
  ==============================================================================

    FILE: BankManager.cpp
    PROJECT: SONAR MIDI PLAYER
    MODULE: backend/BankManager
    DESCRIPTION: Implementation of bank management logic.
                 Stores data in AppData/Roaming/SonarMidiPlayer.

  ==============================================================================
*/

#include "BankManager.h"

// ======================================================
// Konstruktor
// ======================================================
BankManager::BankManager()
{
    loadConfig(); // AI: Načte banky hned při startu
}

// ======================================================
// Cesta k souboru
// ======================================================
juce::File BankManager::getSettingsFile()
{
    // AI: Centrální složka v AppData (profesionální standard)
    auto appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto settingsDir = appData.getChildFile("SonarMidiPlayer");

    if (!settingsDir.exists())
        settingsDir.createDirectory();

    return settingsDir.getChildFile("bank_settings.xml");
}

// ======================================================
// Logika správy bank
// ======================================================

void BankManager::addBank(const juce::File &file)
{
    if (!file.existsAsFile())
        return;

    // AI: Kontrola, zda již banka v seznamu není
    for (const auto &b : loadedBanks)
    {
        if (b.getFullPathName() == file.getFullPathName())
            return;
    }

    loadedBanks.push_back(file);
    saveConfig(); // AI: Uložit změnu okamžitě
}

void BankManager::removeBank(int index)
{
    if (index >= 0 && index < (int)loadedBanks.size())
    {
        loadedBanks.erase(loadedBanks.begin() + index);
        saveConfig(); // AI: Uložit změnu okamžitě
    }
}

// ======================================================
// Načítání a Ukládání (Persistence)
// ======================================================

void BankManager::saveConfig()
{
    juce::XmlElement xml("SONAR_BANK_STATE");

    for (auto &f : loadedBanks)
    {
        auto *e = xml.createNewChildElement("BANK_ENTRY");
        e->setAttribute("fullPath", f.getFullPathName());
    }

    auto file = getSettingsFile();
    xml.writeTo(file);

    DBG(u8"BankManager: Seznam uložen: " << file.getFullPathName());
}

void BankManager::loadConfig()
{
    auto file = getSettingsFile();

    if (!file.exists())
        return;

    auto xml = juce::XmlDocument::parse(file);

    if (xml != nullptr)
    {
        loadedBanks.clear();

        for (auto *e : xml->getChildIterator())
        {
            if (e->hasTagName("BANK_ENTRY"))
            {
                juce::File f(e->getStringAttribute("fullPath"));

                // AI: Přidáme pouze pokud soubor na disku stále existuje
                if (f.existsAsFile())
                    loadedBanks.push_back(f);
            }
        }
    }
}