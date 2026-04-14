/*
  ==============================================================================
    FILE: MidiMapper.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Mapování pomocí TSF API s diagnostickými logy.
    UPDATED: Relativní cesta k tsf.h a trasování indexů.
  ==============================================================================
*/

#include "MidiMapper.h"

// Opravená cesta podle tvé struktury
#include "../MidiPlayer/tsf.h"

#include <iostream>
#include <iomanip>

MidiMapper::MidiMapper(struct tsf *fontInstance) : g_tinyfont(fontInstance) {}
MidiMapper::~MidiMapper() {}

int MidiMapper::findDeepPresetIndex(int bankMSB, int prog, int channel, MidiMode mode)
{
    if (g_tinyfont == nullptr)
    {
        std::cout << "[MAPPER] !!! ERROR: g_tinyfont je NULL" << std::endl;
        return -1;
    }

    int targetBank = (channel == 9) ? 128 : bankMSB;
    int targetProg = prog;

    // Speciální XG/GS logika pro bicí sady
    if (mode == MidiMode::XG && (bankMSB == 126 || bankMSB == 127))
        targetBank = 128;

    // --- LOG VSTUPU ---
    // std::cout << "[MAPPER] Hledám -> Ch: " << std::setw(2) << (channel + 1)
    //          << " | Bank: " << std::setw(3) << targetBank
    //          << " | Prog: " << std::setw(3) << targetProg;

    // 1. POKUS: Přesná shoda (Banka + Program)
    int tsfIndex = tsf_get_presetindex(g_tinyfont, targetBank, targetProg);

    // 2. POKUS: Fallback na Bank 0 (GM), pokud v cílové bance nic není
    if (tsfIndex < 0 && targetBank != 0)
    {
        // std::cout << " [Fallback na Bank 0]";
        tsfIndex = tsf_get_presetindex(g_tinyfont, 0, targetProg);
    }

    // --- VÝSTUPNÍ LOG ---
    if (tsfIndex >= 0)
    {
        // std::cout << " -> OK (TSF_Idx: " << tsfIndex << ")" << std::endl;
        return tsfIndex;
    }
    else
    {
        // std::cout << " -> NENALEZENO (Vracím 0 - Piano)" << std::endl;
        return 0; // Poslední záchrana, aby nebylo ticho
    }
}

MidiMode MidiMapper::detectModeFromSysEx(const uint8_t *data, int size)
{
    if (!data || size < 6)
        return MidiMode::GM;

    // Hledáme ID výrobců: 0x43 (Yamaha - XG), 0x41 (Roland - GS)
    for (int i = 0; i < size - 4; ++i)
    {
        if (data[i] == 0x43)
        {
            std::cout << "[MAPPER] Detekován režim: YAMAHA XG" << std::endl;
            return MidiMode::XG;
        }
        if (data[i] == 0x41)
        {
            std::cout << "[MAPPER] Detekován režim: ROLAND GS" << std::endl;
            return MidiMode::GS;
        }
    }
    return MidiMode::GM;
}