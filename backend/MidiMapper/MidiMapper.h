/*
  ==============================================================================
    FILE: MidiMapper.h
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h> // PŘIDÁNO: Aby mapper znal juce::String
#include <cstdint>

struct tsf;

enum class MidiMode
{
    GM,
    GS,
    XG
};

class MidiMapper
{
public:
    MidiMapper(struct tsf *fontInstance);
    ~MidiMapper();

    int findDeepPresetIndex(int bankMSB, int prog, int channel, MidiMode mode);
    static MidiMode detectModeFromSysEx(const uint8_t *data, int size);

    void updateTSFInstance(struct tsf *newTsf)
    {
        g_tinyfont = newTsf;
    }

private:
    struct tsf *g_tinyfont;
};