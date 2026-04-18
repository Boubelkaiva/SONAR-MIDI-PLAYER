/*
 ==============================================================================
    FILE: FileHash64.h
    MODULE: Hash Utils / Bank DNA
    PROJECT: SONAR MIDI PLAYER (FluidSynth Edition)

    DESCRIPTION:
    Wrapper nad xxHash64 pro generování stabilního ID ze SF2 souborů.

    PURPOSE:
    - generování stabilního ID bank (bank_info.id)
    - identifikace SF2 souborů nezávislá na názvu a cestě

    NOTE:
    Používá externí implementaci XXH64 (xxhash library).
 ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <stdint.h>

// =========================
// EXTERNÍ xxHash API
// =========================
extern "C"
{
    uint64_t XXH64(const void *input, size_t length, unsigned long long seed);
}

// =========================
// FILE HASH FUNKCE
// =========================
int64_t computeFileHash64(const juce::File &file);