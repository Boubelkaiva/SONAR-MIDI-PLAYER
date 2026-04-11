/*
  ==============================================================================
    FILE: MidiAnalyzer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Deep Event List scanning with TSF Index verification.
    UPDATED: Oprava getRawDataSize pro SysEx analýzu.
  ==============================================================================
*/

#include "MidiAnalyzer.h"
#include "../MidiMapper/MidiMapper.h"
#include <iostream>
#include <iomanip>

std::vector<TrackData> MidiAnalyzer::analyzeFile(const juce::File &file, MidiMapper *mapper)
{
    std::vector<TrackData> results;

    // 1. Inicializace 16 slotů (Indexy 0-15 odpovídají MIDI kanálům 1-16)
    for (int i = 0; i < 16; ++i)
    {
        TrackData d;
        d.channel = i + 1;
        d.programNumber = 0;
        d.bankMSB = 0;
        d.bankLSB = 0;
        d.tsfIndex = -1;
        d.instrumentName = (i == 9 ? "Standard Drums" : "Acoustic Piano");
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
        // --- 1. DETEKCE MÓDU (GM / GS / XG) ---
        MidiMode detectedMode = MidiMode::GM;

        for (int t = 0; t < mf.getNumTracks(); ++t)
        {
            auto *seq = mf.getTrack(t);
            for (auto *m : *seq)
            {
                auto &msg = m->message;
                if (msg.isSysEx())
                {
                    // OPRAVA: V JUCE používáme getRawData a getRawDataSize
                    const uint8_t *data = msg.getRawData();
                    int size = msg.getRawDataSize();

                    MidiMode mMode = MidiMapper::detectModeFromSysEx(data, size);
                    if (mMode != MidiMode::GM)
                    {
                        detectedMode = mMode;
                        break;
                    }
                }
            }
            if (detectedMode != MidiMode::GM)
                break;
        }

        std::cout << "\n--- [START ANALÝZY EVENT LISTU] ---" << std::endl;
        std::cout << "[INFO] Detekovaný mód souboru: "
                  << (detectedMode == MidiMode::XG ? "Yamaha XG" : (detectedMode == MidiMode::GS ? "Roland GS" : "General MIDI")) << std::endl;

        // --- 2. PRŮCHOD TRACKY A DEEP SCAN ---
        for (int t = 0; t < mf.getNumTracks(); ++t)
        {
            auto *seq = mf.getTrack(t);
            for (auto *m : *seq)
            {
                auto &msg = m->message;
                int chRaw = msg.getChannel();

                if (chRaw >= 1 && chRaw <= 16)
                {
                    int chIdx = chRaw - 1;

                    if (msg.isController())
                    {
                        int ctrl = msg.getControllerNumber();
                        int val = msg.getControllerValue();

                        if (ctrl == 0)
                            results[chIdx].bankMSB = val;
                        else if (ctrl == 32)
                            results[chIdx].bankLSB = val;
                        else if (ctrl == 7)
                            results[chIdx].initialVolume = (float)val;
                    }
                    else if (msg.isProgramChange())
                    {
                        int prog = msg.getProgramChangeNumber();
                        results[chIdx].programNumber = prog;

                        // Zjištění TSF Indexu pomocí Mapperu
                        int tsfIdx = -1;
                        if (mapper != nullptr)
                        {
                            tsfIdx = mapper->findDeepPresetIndex(results[chIdx].bankMSB, prog, chIdx, detectedMode);
                        }

                        results[chIdx].tsfIndex = tsfIdx;

                        bool isDrum = (chRaw == 10 || results[chIdx].bankMSB >= 126);
                        results[chIdx].instrumentName = getGMName(prog, results[chIdx].bankMSB, isDrum);

                        std::cout << "[DEBUG] Ch " << std::setw(2) << chRaw
                                  << " | Prog: " << std::setw(3) << prog
                                  << " | Bank: " << std::setw(3) << results[chIdx].bankMSB
                                  << " | TSF_Idx: " << std::setw(5) << tsfIdx
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