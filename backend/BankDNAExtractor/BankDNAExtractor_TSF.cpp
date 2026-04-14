/*
  ==============================================================================
    COMPONENT: BankDNAExtractor.cpp
    HYBRIDNÍ EXTRAKCE: Inteligentní rozlišení Roland vs. Yamaha
  ==============================================================================
*/

#include "BankDNAExtractor.h"
#include <fstream>
#include <iostream>

extern "C"
{
#include "../MidiPlayer/tsf.h"
}

bool BankDNAExtractor::extractToJSON(const juce::File &sf2File)
{
    if (!sf2File.existsAsFile())
        return false;

    tsf *tempFont = tsf_load_filename(sf2File.getFullPathName().toRawUTF8());
    if (!tempFont)
        return false;

    juce::File jsonFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                              .getChildFile("SonarMidiPlayer")
                              .getChildFile("bank_settings")
                              .getChildFile(sf2File.getFileNameWithoutExtension() + ".json");

    std::ofstream outFile(jsonFile.getFullPathName().toRawUTF8());
    if (outFile.is_open())
    {
        int count = tsf_get_presetcount(tempFont);
        juce::String fileName = sf2File.getFileName().toLowerCase();

        outFile << "{\n  \"bank_info\": {\n";
        outFile << "    \"totalInstruments\": " << count << "\n  },\n";
        outFile << "  \"instruments\": [\n";

        for (int i = 0; i < count; ++i)
        {
            const char *name = tsf_get_presetname(tempFont, i);
            juce::String instrName = juce::String(name ? name : "Empty");

            int b = 0; // Banka
            int p = 0; // Program

            // --- HYBRIDNÍ LOGIKA ---

            // 1. Detekce BICÍCH (Platí pro oba výrobce)
            if (instrName.containsIgnoreCase("Kit") || instrName.containsIgnoreCase("Drum") || instrName.containsIgnoreCase("Perc"))
            {
                b = 128;     // Tvoje výjimka pro Drums
                p = i % 128; // Program v rámci bicí banky
            }
            // 2. Specifická logika pro YAMAHA (XG hybrid)
            else if (fileName.contains("yamaha") || fileName.contains("_xg"))
            {
                // Yamaha XG často používá lineární indexování, které musíme rozsekat:
                b = i / 128; // Každých 128 nástrojů nová banka
                p = i % 128; // Program 0-127
            }
            // 3. Specifická logika pro ROLAND (GM/GS hybrid)
            else
            {
                // Roland se většinou drží banky 0, pokud index nepřeleze limit
                if (i < 128)
                {
                    b = 0;
                    p = i;
                }
                else
                {
                    b = 1; // Variace u Rolanda
                    p = i - 128;
                }
            }

            juce::String safeName = instrName.replace("\"", "\\\"");

            outFile << "    {\n";
            outFile << "      \"idx\": " << i << ",\n";
            outFile << "      \"bank\": " << b << ",\n";
            outFile << "      \"prog\": " << p << ",\n";
            outFile << "      \"name\": \"" << safeName.toRawUTF8() << "\"\n";
            outFile << "    }" << (i < count - 1 ? "," : "") << "\n";
        }
        outFile << "  ]\n}";
        outFile.close();
    }

    tsf_close(tempFont);
    std::cout << "[AI] Hybridni DNA ulozena pro: " << sf2File.getFileName() << std::endl;
    return true;
}