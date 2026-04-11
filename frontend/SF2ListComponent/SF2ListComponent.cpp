/*
  ==============================================================================

    FILE: SF2ListComponent.cpp
    PROJECT: SONAR MIDI PLAYER
    MODULE: frontend/SF2ListComponent
    DESCRIPTION: SF2 list implementation.
                 Now fully integrated with BankManager (Backend).
                 Includes automatic DNA extraction to JSON during load.

  ==============================================================================
*/

#include "SF2ListComponent.h"
#include "../../backend/BankDNAExtractor/BankDNAExtractor.h" // AI: Přidán include pro extraktor

// ======================================================
// Konstruktor - Propojení s Backendem
// ======================================================
SF2ListComponent::SF2ListComponent(BankManager &manager) : bankManager(manager)
{
    // 1. Tlačítko FOLDER BANK (Nahoře)
    addAndMakeVisible(chooseFolderButton);
    chooseFolderButton.setButtonText(u8"FOLDER BANK");
    chooseFolderButton.onClick = [this]
    {
        juce::File startDir = lastBankDirectory.exists() ? lastBankDirectory
                                                         : juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems;

        fc = std::make_unique<juce::FileChooser>(u8"Vyberte SF2 banky...", startDir, "*.sf2");

        fc->launchAsync(chooserFlags, [this](const juce::FileChooser &chooser)
                        {
            auto results = chooser.getResults();
            if (results.size() > 0)
            {
                lastBankDirectory = results[0].getParentDirectory();
                for (auto& file : results)
                {
                    if (file.existsAsFile()) loadSF2(file);
                }
            } });
    };

    // 2. Tlačítko DELETE BANK (Nahoře)
    addAndMakeVisible(deleteBankButton);
    deleteBankButton.setButtonText(u8"DELETE BANK");
    deleteBankButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.8f));
    deleteBankButton.onClick = [this]
    { deleteSelectedBank(); };

    // 3. Nastavení tabulky (Pod tlačítky)
    addAndMakeVisible(sf2List);
    sf2List.getHeader().addColumn(u8"SF2 Banky (Seznam)", 1, 200);
    sf2List.setModel(this);
    sf2List.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    sf2List.setMultipleSelectionEnabled(false);
}

SF2ListComponent::~SF2ListComponent() {}

// ======================================================
// TableListBoxModel Implementace (Čte z Manageru)
// ======================================================

int SF2ListComponent::getNumRows()
{
    return (int)bankManager.getLoadedBanks().size();
}

void SF2ListComponent::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::azure.withAlpha(0.15f));
    else if (rowNumber % 2 == 0)
        g.fillAll(juce::Colours::black.withAlpha(0.1f));
}

void SF2ListComponent::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(rowIsSelected ? juce::Colours::yellow : juce::Colours::white);

    auto &currentBanks = bankManager.getLoadedBanks();

    if (rowNumber < (int)currentBanks.size())
    {
        g.drawText(currentBanks[rowNumber].getFileNameWithoutExtension(),
                   5, 0, width - 10, height,
                   juce::Justification::centredLeft, true);
    }
}

// ======================================================
// Logika správy bank (Volá Backend)
// ======================================================

void SF2ListComponent::loadSF2(const juce::File &file)
{
    // --- AI DEBUG START ---
    juce::Logger::writeToLog("--------------------------------------------------");
    juce::Logger::writeToLog("[AI DEBUG] Zahajuji proces pro: " + file.getFileName());

    // 1. KROK: Blesková extrakce DNA (vytvoří JSON v AppData/bank_settings)
    if (BankDNAExtractor::extractToJSON(file))
    {
        // Najdeme cestu pro potvrzení v logu
        juce::File jsonPath = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                  .getChildFile("SonarMidiPlayer")
                                  .getChildFile("bank_settings")
                                  .getChildFile(file.getFileNameWithoutExtension() + ".json");

        juce::Logger::writeToLog("[AI SUCCESS] JSON DNA vytvořen: " + jsonPath.getFullPathName());
    }
    else
    {
        juce::Logger::writeToLog("[AI ERROR] Extrakce DNA selhala pro: " + file.getFullPathName());
    }
    // --- AI DEBUG END ---

    // 2. KROK: BankManager si sám hlídá duplicity a ukládání do XML
    bankManager.addBank(file);

    sf2List.updateContent();
    sf2List.repaint();
}

void SF2ListComponent::deleteSelectedBank()
{
    int selected = sf2List.getSelectedRow();
    auto &currentBanks = bankManager.getLoadedBanks();

    if (selected >= 0 && selected < (int)currentBanks.size())
    {
        bankManager.removeBank(selected);
        sf2List.updateContent();
        sf2List.deselectAllRows();
        sf2List.repaint();
    }
}

void SF2ListComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
}

// ======================================================
// ROZLOŽENÍ PRVKŮ (Layout)
// ======================================================

void SF2ListComponent::resized()
{
    auto area = getLocalBounds().reduced(5);

    // 1. Prostor pro tlačítka NAHOŘE (výška 35px)
    auto topRow = area.removeFromTop(35);

    int gap = 4;
    int buttonWidth = (topRow.getWidth() - gap) / 2;

    chooseFolderButton.setBounds(topRow.removeFromLeft(buttonWidth));
    deleteBankButton.setBounds(topRow.removeFromRight(buttonWidth));

    // 2. Mezera mezi tlačítka a tabulku
    area.removeFromTop(5);

    // 3. Tabulka zabere zbytek
    sf2List.setBounds(area);
}

// ======================================================
// Drag & Drop
// ======================================================

bool SF2ListComponent::isInterestedInFileDrag(const juce::StringArray &files)
{
    for (auto &f : files)
        if (f.endsWith(".sf2"))
            return true;
    return false;
}

void SF2ListComponent::filesDropped(const juce::StringArray &files, int x, int y)
{
    juce::ignoreUnused(x, y);
    for (auto &f : files)
    {
        juce::File file(f);
        if (file.existsAsFile() && file.hasFileExtension("sf2"))
            loadSF2(file);
    }
}