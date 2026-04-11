/*
  ==============================================================================
    COMPONENT: BankDNAExtractor.cpp
    MODULE: backend/BankDNAExtractor/BankDNAExtractor.cpp
    DESCRIPTION: Implementation of the DNA Extraction logic.
                 FIXED: Handling "incomplete type tsf" without duplicate symbols.
    AUTHOR: Iva
  ==============================================================================
*/

#include "BankDNAExtractor.h"
#include <fstream>
#include <iostream>

// AI: Tady je ten trik. Potřebujeme vidět STRUKTURY, ale nechceme FUNKCE (ty jsou v MidiPlayeru).
// Většina single-header knihoven (včetně TSF) při běžném include bez
// TSF_IMPLEMENTATION ukáže jen veřejné API a struktury nechá "nekompletní".
extern "C"
{
#include "../MidiPlayer/tsf.h"
}

bool BankDNAExtractor::extractToJSON(const juce::File &sf2File)
{
    if (!sf2File.existsAsFile())
        return false;

    // 1. Načtení banky
    // Používáme pouze veřejné API (funkce), abychom se vyhnuli problémům s typem.
    tsf *tempFont = tsf_load_filename(sf2File.getFullPathName().toRawUTF8());

    if (tempFont == nullptr)
    {
        std::cerr << "[BankDNAExtractor] Chyba: SF2 nelze načíst." << std::endl;
        return false;
    }

    // 2. Příprava JSONu
    juce::File jsonFile = sf2File.withFileExtension(".json");
    std::ofstream outFile(jsonFile.getFullPathName().toRawUTF8());

    if (!outFile.is_open())
    {
        tsf_close(tempFont);
        return false;
    }

    // Abychom se vyhnuli chybě "nekompletní typ tsf" u tempFont->presets,
    // musíme použít pouze VEŘEJNÉ FUNKCE, které TSF nabízí.

    int presetCount = tsf_get_presetcount(tempFont);

    outFile << "{\n";
    outFile << "  \"bank_info\": {\n";
    outFile << "    \"fileName\": \"" << sf2File.getFileName().toRawUTF8() << "\",\n";
    outFile << "    \"totalInstruments\": " << presetCount << "\n";
    outFile << "  },\n";
    outFile << "  \"instruments\": [\n";

    for (int i = 0; i < presetCount; ++i)
    {
        const char *name = tsf_get_presetname(tempFont, i);

        // TSF nemá tsf_get_preset_bank(f, index), ale má tsf_get_presetindex(f, bank, preset).
        // Protože potřebujeme opačný směr a struktura tsf je v tomto souboru nekompletní,
        // musíme se podívat na vnitřní strukturu tsf_preset, která je v tsf.h definovaná.
        struct tsf_internal
        {
            struct
            {
                int bank, preset;
                const char *name;
            } *presets;
        };
        int bank = ((struct tsf_internal *)tempFont)->presets[i].bank;
        int prog = ((struct tsf_internal *)tempFont)->presets[i].preset;

        juce::String cleanName = juce::String(name ? name : "Unknown");
        cleanName = cleanName.replace("\"", "\\\"");

        outFile << "    {\n";
        outFile << "      \"idx\": " << i << ",\n";
        outFile << "      \"bank\": " << bank << ",\n";
        outFile << "      \"prog\": " << prog << ",\n";
        outFile << "      \"name\": \"" << cleanName.toRawUTF8() << "\"\n";
        outFile << "    }";

        if (i < presetCount - 1)
            outFile << ",";
        outFile << "\n";
    }

    outFile << "  ]\n}";

    outFile.close();
    tsf_close(tempFont);
    return true;
}