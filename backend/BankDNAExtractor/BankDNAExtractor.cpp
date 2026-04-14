/*
  ==============================================================================
    COMPONENT: BankDNAExtractor.cpp
    PROJECT: SONAR MIDI PLAYER (FluidSynth Edition)
    HYBRIDNÍ EXTRAKCE: Inteligentní rozlišení Roland vs. Yamaha přes FluidSynth API
  ==============================================================================
*/

#include "BankDNAExtractor.h"
#include <fluidsynth.h>
#include <fstream>
#include <iostream>

bool BankDNAExtractor::extractToJSON(const juce::File &sf2File)
{
    if (!sf2File.existsAsFile())
        return false;

    // Inicializace FluidSynth pouze pro účely extrakce
    fluid_settings_t *settings = new_fluid_settings();
    fluid_synth_t *synth = new_fluid_synth(settings);

    int sfont_id = fluid_synth_sfload(synth, sf2File.getFullPathName().toRawUTF8(), 1);
    if (sfont_id == FLUID_FAILED)
    {
        delete_fluid_synth(synth);
        delete_fluid_settings(settings);
        return false;
    }

    fluid_sfont_t *sfont = fluid_synth_get_sfont_by_id(synth, (unsigned int)sfont_id);

    juce::File jsonFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                              .getChildFile("SonarMidiPlayer")
                              .getChildFile("bank_settings")
                              .getChildFile(sf2File.getFileNameWithoutExtension() + ".json");

    if (!jsonFile.getParentDirectory().exists())
        jsonFile.getParentDirectory().createDirectory();

    std::ofstream outFile(jsonFile.getFullPathName().toRawUTF8());
    if (outFile.is_open())
    {
        juce::String fileName = sf2File.getFileName().toLowerCase();

        // FluidSynth nemá přímý "count" jako TSF, projdeme presety iterátorem
        outFile << "{\n  \"bank_info\": {\n";
        outFile << "    \"source\": \"" << sf2File.getFileName().toRawUTF8() << "\"\n  },\n";
        outFile << "  \"instruments\": [\n";

        fluid_sfont_iteration_start(sfont);
        fluid_preset_t *preset;
        int internalIdx = 0;
        bool first = true;

        while ((preset = fluid_sfont_iteration_next(sfont)))
        {
            if (!first)
                outFile << ",\n";
            first = false;

            const char *name = fluid_preset_get_name(preset);
            juce::String instrName = juce::String(name ? name : "Empty");

            int b = fluid_preset_get_banknum(preset); // Skutečná banka ze SoundFontu
            int p = fluid_preset_get_num(preset);     // Skutečný program

            // --- TVÁ HYBRIDNÍ LOGIKA (ZACHOVÁNA A ADAPTOVÁNA) ---

            // 1. Detekce BICÍCH (Prioritní pravidlo)
            if (instrName.containsIgnoreCase("Kit") || instrName.containsIgnoreCase("Drum") || instrName.containsIgnoreCase("Perc"))
            {
                // Vynutíme banku 128 pro tvůj systém bicích, pokud již není
                b = 128;
            }
            // 2. Specifická logika pro YAMAHA (XG hybrid)
            else if (fileName.contains("yamaha") || fileName.contains("_xg"))
            {
                // U XG souborů FluidSynth obvykle banky přečte správně,
                // ale pokud by byly lineární (jako v TSF), zachováváme tvůj propočet:
                if (b == 0 && internalIdx >= 128)
                {
                    b = internalIdx / 128;
                    p = internalIdx % 128;
                }
            }
            // 3. Specifická logika pro ROLAND (GM/GS hybrid)
            else
            {
                // Roland variace: Pokud FluidSynth vidí banku 0, ale index je vysoký,
                // aplikujeme tvůj posun pro GS variace.
                if (b == 0 && internalIdx >= 128)
                {
                    b = 1;
                    p = internalIdx - 128;
                }
            }

            juce::String safeName = instrName.replace("\"", "\\\"");

            outFile << "    {\n";
            outFile << "      \"idx\": " << internalIdx << ",\n";
            outFile << "      \"bank\": " << b << ",\n";
            outFile << "      \"prog\": " << p << ",\n";
            outFile << "      \"name\": \"" << safeName.toRawUTF8() << "\"\n";
            outFile << "    }";

            internalIdx++;
        }

        outFile << "\n  ]\n}";
        outFile.close();
    }

    delete_fluid_synth(synth);
    delete_fluid_settings(settings);

    std::cout << "[AI] Fluid Hybridni DNA ulozena: " << sf2File.getFileName() << std::endl;
    return true;
}