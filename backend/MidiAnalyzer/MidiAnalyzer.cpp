/*
  ==============================================================================
    FILE: MidiAnalyzer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Deep Event List scanning with TSF Index verification.
    UPDATED: Integration with MidiMapper for real-time index debugging.
  ==============================================================================
*/

#include "MidiAnalyzer.h"
#include "../MidiMapper/MidiMapper.h" // Musíme vidět na Mapper
#include <iostream>
#include <iomanip>

// Předpokládám, že máš v MidiAnalyzer přístup k instanci Mapperu,
// nebo ho předáme v metodě. Pro tento výpis ho vytvoříme lokálně nebo použijeme externí.

std::vector<TrackData> MidiAnalyzer::analyzeFile(const juce::File &file, MidiMapper *mapper)
{
    std::vector<TrackData> results;

    // 1. Inicializace 16 slotů (Kanály 1-16)
    for (int i = 1; i <= 16; ++i)
    {
        TrackData d;
        d.channel = i;
        d.programNumber = 0;
        d.bankMSB = 0;
        d.bankLSB = 0;
        d.instrumentName = (i == 10 ? "Standard Drums" : "Acoustic Piano");
        d.initialVolume = 100.0f;
        results.push_back(d);
    }

    if (!file.existsAsFile())
        return results;

    juce::FileInputStream inputStream(file);
    if (!inputStream.openedOk())
        return results;

    juce::MidiFile mf;
    if (mf.readFrom(inputStream))
    {
        std::cout << "\n--- [START ANALÝZY EVENT LISTU] ---" << std::endl;

        for (int t = 0; t < mf.getNumTracks(); ++t)
        {
            auto *seq = mf.getTrack(t);
            for (auto *m : *seq)
            {
                auto &msg = m->message;
                int ch = msg.getChannel();

                if (ch >= 1 && ch <= 16)
                {
                    int chIdx = ch - 1;

                    if (msg.isController())
                    {
                        int ctrl = msg.getControllerNumber();
                        int val = msg.getControllerValue();

                        if (ctrl == 0)
                        {
                            results[chIdx].bankMSB = val;
                        }
                        else if (ctrl == 7)
                        {
                            results[chIdx].initialVolume = (float)val;
                        }
                    }
                    else if (msg.isProgramChange())
                    {
                        int prog = msg.getProgramChangeNumber();
                        results[chIdx].programNumber = prog;

                        // Zjištění TSF Indexu pomocí Mapperu
                        int tsfIdx = -1;
                        if (mapper != nullptr)
                        {
                            // Simulujeme GM mód pro analýzu
                            tsfIdx = mapper->findDeepPresetIndex(results[chIdx].bankMSB, prog, chIdx, MidiMode::GM);
                        }

                        bool isDrum = (ch == 10 || results[chIdx].bankMSB >= 126);
                        results[chIdx].instrumentName = getGMName(prog, results[chIdx].bankMSB, isDrum);

                        // FORMÁTOVANÝ VÝPIS PRO IVU (Vše na jednom řádku)
                        std::cout << "[DEBUG] Ch " << std::setw(2) << ch
                                  << " | Prog: " << std::setw(3) << prog
                                  << " | Bank: " << std::setw(3) << results[chIdx].bankMSB
                                  << " | TSF_Idx: " << std::setw(4) << tsfIdx // Tady uvidíš tu pravdu!
                                  << " | Instrument: " << results[chIdx].instrumentName << std::endl;
                    }
                }
            }
        }
        std::cout << "--- [KONEC ANALÝZY] ---\n"
                  << std::endl;
    }

    return results;
}

juce::String MidiAnalyzer::getGMName(int p, int bank, bool isDrumChannel)
{
    if (isDrumChannel || bank >= 126)
        return "Drum Kit (" + juce::String(p) + ")";

    if (p >= 0 && p <= 7)
        return "Piano";
    if (p >= 8 && p <= 15)
        return "Chr. Perc.";
    if (p >= 16 && p <= 23)
        return "Organ";
    if (p >= 24 && p <= 31)
        return "Guitar";
    if (p >= 32 && p <= 39)
        return "Bass";
    if (p >= 40 && p <= 47)
        return "Strings";
    if (p >= 48 && p <= 55)
        return "Ensemble";
    if (p >= 56 && p <= 63)
        return "Brass";
    if (p >= 64 && p <= 71)
        return "Reed";
    if (p >= 72 && p <= 79)
        return "Pipe";
    if (p >= 80 && p <= 87)
        return "Synth Lead";
    if (p >= 88 && p <= 95)
        return "Synth Pad";

    return "Instr. " + juce::String(p + 1);
}