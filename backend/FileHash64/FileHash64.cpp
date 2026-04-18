/*
 ==============================================================================
    FILE: FileHash64.cpp
    MODULE: External Library / Hashing
    PROJECT: SONAR MIDI PLAYER (FluidSynth Edition)
    DESCRIPTION: Implementace xxHash64 wrapperu pro generování stabilních ID.
                 Používá se pro bank_info.id (SF2 identifikace).
 ==============================================================================
*/

#include "FileHash64.h"
#include <juce_core/juce_core.h>

// ✅ Přidáno: inline implementace xxHash (žádný linker problém)
#define XXH_INLINE_ALL
#include "xxhash.h"

int64_t computeFileHash64(const juce::File &file)
{
    juce::MemoryBlock data;

    if (!file.loadFileAsData(data))
        return 0;

    return static_cast<int64_t>(
        XXH64(data.getData(), data.getSize(), 0));
}