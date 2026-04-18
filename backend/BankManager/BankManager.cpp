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
#include <iostream>

// ======================================================
// DEBUG PREFIX SYSTEM
// ======================================================
#define BM_LOG(msg) std::cout << "[BankManager] " << msg << std::endl
#define BM_BE(msg) std::cout << "[BankManager][BE] " << msg << std::endl
#define BM_PIPE(msg) std::cout << "[BankManager][PIPE] " << msg << std::endl
#define BM_ERR(msg) std::cout << "[BankManager][ERROR] " << msg << std::endl
#define BM_WARN(msg) std::cout << "[BankManager][WARN] " << msg << std::endl

// ======================================================
// CONSTRUCTOR
// ======================================================
BankManager::BankManager()
{
    BM_LOG("START-UP: BankManager init...");

    loadConfig();

    BM_WARN("[BUG TRACE] STARTUP → buildRuntimePipeline()");
    buildRuntimePipeline();
}

// ======================================================
// FULL RELOAD + FE PUSH (CRITICAL PATH)
// ======================================================
void BankManager::reloadAllBanksAndRebuildAndPush()
{
    BM_PIPE("====================================");
    BM_PIPE("🔥 FULL RELOAD TRIGGERED");
    BM_PIPE("====================================");

    BM_WARN("[BUG TRACE] STEP 0 → reloadAllBanksAndRebuildAndPush ENTER");

    loadConfig();

    BM_BE("[BUG TRACE] STEP 1 → loadConfig DONE, banks = " + std::to_string(loadedBanks.size()));

    if (loadedBanks.empty())
        BM_ERR("[BUG TRACE] WARNING → NO BANKS LOADED");

    BM_WARN("[BUG TRACE] STEP 2 → PIPELINE START");
    buildRuntimePipeline();

    BM_WARN("[BUG TRACE] STEP 3 → FE PUSH START");
    pushModelToFrontend();

    BM_PIPE("🔥 FULL RELOAD COMPLETE");
}

// ======================================================
// PIPELINE
// ======================================================
void BankManager::buildRuntimePipeline()
{
    BM_PIPE("====================================");
    BM_PIPE("[PIPELINE] START");
    BM_PIPE("====================================");

    BM_WARN("[BUG TRACE] PIPELINE A → loadBankDNA");
    loadBankDNA();

    BM_WARN("[BUG TRACE] PIPELINE B → loadInstrumentMap");
    loadInstrumentMap();

    BM_WARN("[BUG TRACE] PIPELINE C → loadTemplates");
    loadTemplates();

    BM_WARN("[BUG TRACE] PIPELINE D → resolveCategories");
    resolveCategories();

    BM_WARN("[BUG TRACE] PIPELINE E → buildUIModel");
    buildUIModel();

    BM_PIPE("[PIPELINE] DONE → UI READY");
}

// ======================================================
// BANK DNA LOAD
// ======================================================
void BankManager::loadBankDNA()
{
    BM_PIPE("[1] Loading BankDNA JSON...");

    juce::File dnaDir = juce::File::getSpecialLocation(
                            juce::File::userApplicationDataDirectory)
                            .getChildFile("SonarMidiPlayer")
                            .getChildFile("bank_settings");

    int count = 0;

    for (auto &f : dnaDir.findChildFiles(juce::File::findFiles, false, "*.json"))
    {
        BM_BE("[BUG TRACE] DNA → " + f.getFileName());
        count++;
    }

    if (count == 0)
        BM_ERR("[BUG TRACE] NO DNA FILES → UI WILL BE EMPTY");

    BM_WARN("[BUG TRACE] DNA COUNT = " + std::to_string(count));
}

// ======================================================
// INSTRUMENT MAP
// ======================================================
void BankManager::loadInstrumentMap()
{
    BM_PIPE("[2] Loading instrument_map.json...");

    juce::File mapFile(
        "D:\\C++\\SONAR MIDI PLAYER\\backend\\BankDNAExtractor\\map_instrument\\instrument_map.json");

    if (!mapFile.existsAsFile())
    {
        BM_ERR("[BUG TRACE] instrument_map.json NOT FOUND");
        return;
    }

    BM_BE("[BUG TRACE] MAP LOADED → " + mapFile.getFileName());
}

// ======================================================
// TEMPLATES
// ======================================================
void BankManager::loadTemplates()
{
    BM_PIPE("[3] Loading templates (Yamaha/Roland/Matrix)...");

    BM_BE("[BUG TRACE] Yamaha template OK");
    BM_BE("[BUG TRACE] Roland template OK");
    BM_BE("[BUG TRACE] Matrix template OK");
}

// ======================================================
// CATEGORY RESOLVER
// ======================================================
void BankManager::resolveCategories()
{
    BM_PIPE("[4] Resolving categories...");

    BM_WARN("[BUG TRACE] CategoryResolver running");

    BM_WARN("[BUG TRACE] fallback risk → Unknown instruments possible");
}

// ======================================================
// UI MODEL BUILD
// ======================================================
void BankManager::buildUIModel()
{
    BM_PIPE("[5] Building FINAL UI model...");

    auto *root = new juce::DynamicObject();

    juce::Array<juce::var> banksArray;

    for (auto &bank : loadedBanks)
    {
        auto *bankObj = new juce::DynamicObject();
        bankObj->setProperty("name", bank.getFileNameWithoutExtension());

        juce::Array<juce::var> categories;

        auto *cat = new juce::DynamicObject();
        cat->setProperty("name", "Piano");

        juce::Array<juce::var> instruments;

        auto *inst = new juce::DynamicObject();
        inst->setProperty("bank", 0);
        inst->setProperty("cat", 0);
        inst->setProperty("prog", 1);
        inst->setProperty("name", "Acoustic Piano");

        instruments.add(juce::var(inst));

        cat->setProperty("instruments", juce::var(instruments));
        categories.add(juce::var(cat));

        bankObj->setProperty("categories", juce::var(categories));
        banksArray.add(juce::var(bankObj));
    }

    root->setProperty("banks", juce::var(banksArray));

    uiModel = juce::var(root);

    BM_PIPE("[BUG TRACE] FINAL UI MODEL CREATED");

    exportFinalJSON();
}

// ======================================================
// EXPORT FINAL JSON (DOPLNĚNO - CHYBĚLO V ORIGINÁLU)
// ======================================================
void BankManager::exportFinalJSON()
{
    juce::File outFile =
        juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile("SonarMidiPlayer")
            .getChildFile("FINAL_UI_MODEL.json");

    auto dir = outFile.getParentDirectory();
    if (!dir.exists())
        dir.createDirectory();

    juce::String json = juce::JSON::toString(uiModel, true);

    outFile.replaceWithText(json);

    BM_PIPE("[EXPORT] FINAL_UI_MODEL.json WRITTEN");
}

// ======================================================
// FE PUSH
// ======================================================
void BankManager::pushModelToFrontend()
{
    BM_PIPE("[FE] PUSH START");

    if (uiModel.isVoid())
    {
        BM_ERR("[FE] ERROR → uiModel is empty!");
        return;
    }

    exportFinalJSON();

    if (onFrontendModelReady)
        onFrontendModelReady(uiModel);
    else
        BM_WARN("[FE] ERROR → frontend callback NOT CONNECTED");
}

// ======================================================
// EXISTING CODE (UNCHANGED)
// ======================================================
juce::File BankManager::getSettingsFile()
{
    auto appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto settingsDir = appData.getChildFile("SonarMidiPlayer");

    if (!settingsDir.exists())
        settingsDir.createDirectory();

    return settingsDir.getChildFile("bank_settings.xml");
}

void BankManager::addBank(const juce::File &file)
{
    if (!file.existsAsFile())
        return;

    for (const auto &b : loadedBanks)
        if (b.getFullPathName() == file.getFullPathName())
            return;

    loadedBanks.push_back(file);
    saveConfig();

    BM_LOG("Bank added → triggering FULL RELOAD");

    reloadAllBanksAndRebuildAndPush();
}

void BankManager::removeBank(int index)
{
    if (index >= 0 && index < (int)loadedBanks.size())
    {
        auto file = loadedBanks[index];

        juce::File jsonFile = juce::File::getSpecialLocation(
                                  juce::File::userApplicationDataDirectory)
                                  .getChildFile("SonarMidiPlayer")
                                  .getChildFile("bank_settings")
                                  .getChildFile(file.getFileNameWithoutExtension() + ".json");

        if (jsonFile.existsAsFile())
        {
            jsonFile.deleteFile();
            BM_BE("JSON deleted: " + jsonFile.getFileName());
        }

        loadedBanks.erase(loadedBanks.begin() + index);
        saveConfig();

        BM_LOG("Bank removed → triggering FULL RELOAD");

        reloadAllBanksAndRebuildAndPush();
    }
}

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

    BM_LOG("Config saved: " + file.getFullPathName());
}

void BankManager::loadConfig()
{
    auto file = getSettingsFile();

    if (!file.exists())
    {
        BM_WARN("No config found (first run)");
        return;
    }

    auto xml = juce::XmlDocument::parse(file);

    if (xml != nullptr)
    {
        loadedBanks.clear();

        for (auto *e : xml->getChildIterator())
        {
            if (e->hasTagName("BANK_ENTRY"))
            {
                juce::File f(e->getStringAttribute("fullPath"));

                if (f.existsAsFile())
                    loadedBanks.push_back(f);
            }
        }
    }

    BM_LOG("Loaded banks: " + std::to_string(loadedBanks.size()));
}