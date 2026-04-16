/*
 ==============================================================================
    FILE: MidiPlayer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: FIXED AUDIO ENGINE (WORKING FLUIDSYNTH PIPELINE)
 ==============================================================================
*/

#include "MidiPlayer.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"
#include <iostream>
#include <thread>
#include <cmath>

#define LOG(x) std::cout << x << std::endl

// =========================
// CONSTRUCTOR
// =========================
MidiPlayer::MidiPlayer()
    : isPlaying(false),
      currentSampleRate(48000.0),
      masterVolume(1.0f),
      playheadSeconds(0.0),
      settings(nullptr),
      synth(nullptr)
{
    std::cout << "[POG] BE: LIFE-CYCLE: Startuji MidiPlayer (Constructor)" << std::endl;

    settings = new_fluid_settings();

    // Umlčení interních varování FluidSynthu (odstraní spam "No preset found")
    fluid_set_log_function(FLUID_WARN, nullptr, nullptr);
    fluid_set_log_function(FLUID_ERR, nullptr, nullptr);

    fluid_settings_setnum(settings, "synth.sample-rate", currentSampleRate);
    fluid_settings_setint(settings, "synth.polyphony", 256);
    fluid_settings_setnum(settings, "synth.gain", 1.0);

    synth = new_fluid_synth(settings);

    fluid_synth_system_reset(synth);

    mapper = std::make_unique<MidiMapper>();

    for (int i = 0; i < 16; ++i)
    {
        currentBankMSB[i] = (i == 9) ? 128 : 0;
        currentBankLSB[i] = 0;
        channelMuted[i] = false;
        channelSolo[i] = false;

        fluid_synth_program_change(synth, i, 0);
    }

    std::cout << "[POG] BE: INIT: FluidSynth připraven na 48kHz, mapper OK." << std::endl;
}

// =========================
// DESTRUCTOR
// =========================
MidiPlayer::~MidiPlayer()
{
    if (synth)
        delete_fluid_synth(synth);

    if (settings)
        delete_fluid_settings(settings);
}

// =========================
// REQUIRED (FIX LINKER ERROR)
// =========================
void MidiPlayer::releaseResources()
{
}

// =========================
// AUDIO INIT
// =========================
void MidiPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (sampleRate <= 0)
        return;

    bool reinit = (std::abs(currentSampleRate - sampleRate) > 0.01);

    if (!reinit)
        return;

    if (synth)
    {
        std::cout << "[POG] BE: AUDIO-CHANGE: Re-inicializace na " << sampleRate << " Hz" << std::endl;
        delete_fluid_synth(synth);
        synth = nullptr;
    }

    currentSampleRate = sampleRate;

    fluid_settings_setnum(settings, "synth.sample-rate", currentSampleRate);

    synth = new_fluid_synth(settings);

    fluid_synth_system_reset(synth);
    fluid_synth_set_gain(synth, 1.0f);

    for (int i = 0; i < 16; ++i)
        fluid_synth_program_change(synth, i, 0);

    if (!lastSf2Path.isEmpty())
    {
        int sfont_id = fluid_synth_sfload(synth, lastSf2Path.toRawUTF8(), 1);
        std::cout << "[POG] BE: SoundFont automaticky znovunačten (ID: " << sfont_id << ")" << std::endl;
    }
}

// =========================
// AUDIO CALLBACK
// =========================
void MidiPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    // --- ZDE NESMÍ BÝT ŽÁDNÝ std::cout (ODSTRANĚNY TIKY) ---

    bufferToFill.clearActiveBufferRegion();

    if (!isPlaying || synth == nullptr)
        return;

    const int samples = bufferToFill.numSamples;
    const double blockStartTime = playheadSeconds;
    const double blockEndTime = blockStartTime + (samples / currentSampleRate);

    // 1. Dispatch MIDI events for this time slice
    for (; lastEventIndex < midiSequence.getNumEvents(); ++lastEventIndex)
    {
        auto *event = midiSequence.getEventPointer(lastEventIndex);
        if (event->message.getTimeStamp() >= blockEndTime)
            break;

        if (event->message.getTimeStamp() >= blockStartTime)
            processMidiMessage(event->message);
    }

    float *left = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float *right = (bufferToFill.buffer->getNumChannels() > 1)
                       ? bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample)
                       : nullptr;

    float *outL = left;
    float *outR = right ? right : left;

    fluid_synth_write_float(
        synth,
        samples,
        outL, 0, 1,
        outR, 0, 1);

    bufferToFill.buffer->applyGain(bufferToFill.startSample, samples, masterVolume);

    // 3. Update playhead
    playheadSeconds = blockEndTime;

    if (playheadSeconds >= midiSequence.getEndTime())
        isPlaying = false;
}

// =========================
// SOUND FONT
// =========================
void MidiPlayer::loadSoundFont(const juce::File &sf2File)
{
    if (!sf2File.existsAsFile())
    {
        std::cout << "[POG] BE: SF2-LOAD: ERR - Soubor neexistuje: " << sf2File.getFileName() << std::endl;
        return;
    }

    lastSf2Path = sf2File.getFullPathName();
    std::cout << "[POG] BE: SF2-LOAD: Pokus o načtení banky -> " << sf2File.getFileName() << std::endl;

    if (synth)
    {
        int id = fluid_synth_sfload(synth, lastSf2Path.toRawUTF8(), 1);
        if (id != -1)
            std::cout << "[POG] BE: SF2-LOAD: ÚSPĚCH! Banka načtena pod ID: " << id << std::endl;
        else
            std::cout << "[POG] BE: SF2-LOAD: KRITICKÁ CHYBA - FluidSynth soubor odmítl!" << std::endl;
    }
}

// =========================
// MIDI LOAD (FIX LINKER)
// =========================
void MidiPlayer::loadMidiFile(const juce::File &midiFile)
{
    if (!midiFile.existsAsFile())
    {
        std::cout << "[POG] BE: MIDI-LOAD: ERR - Soubor nenalezen." << std::endl;
        return;
    }

    std::cout << "[POG] BE: MIDI-LOAD: Otevírám " << midiFile.getFileName() << std::endl;
    juce::FileInputStream inputStream(midiFile);
    if (!inputStream.openedOk())
        return;

    juce::MidiFile mf;
    if (mf.readFrom(inputStream))
    {
        mf.convertTimestampTicksToSeconds();
        midiSequence.clear();
        for (int i = 0; i < mf.getNumTracks(); ++i)
            midiSequence.addSequence(*mf.getTrack(i), 0.0);

        midiSequence.updateMatchedPairs();
        playheadSeconds = 0.0;
        lastEventIndex = 0;
        std::cout << "[POG] BE: MIDI-LOAD: Hotovo. Událostí: " << midiSequence.getNumEvents() << std::endl;
    }
}

void MidiPlayer::applyAnalysisResults(const std::vector<TrackData> &results)
{
    std::cout << "[POG] BE: ANALÝZA: Propojuji výsledky s audio enginem." << std::endl;
}

// =========================
// PLAYBACK CONTROL
// =========================
void MidiPlayer::play()
{
    std::cout << "[POG] BE: TRANSPORT: Kliknuto na PLAY" << std::endl;
    isPlaying = true;

    if (playheadSeconds >= midiSequence.getEndTime())
    {
        playheadSeconds = 0.0;
        lastEventIndex = 0;
    }
}

void MidiPlayer::stop()
{
    std::cout << "[POG] BE: TRANSPORT: Kliknuto na STOP" << std::endl;
    isPlaying = false;
    playheadSeconds = 0.0;
    lastEventIndex = 0;

    if (synth)
        fluid_synth_all_sounds_off(synth, -1);
}

void MidiPlayer::pause()
{
    std::cout << "[POG] BE: TRANSPORT: Kliknuto na PAUSE" << std::endl;
    isPlaying = false;
}

// =========================
// MIDI PROCESS
// =========================
void MidiPlayer::processMidiMessage(const juce::MidiMessage &m)
{
    if (!synth)
        return;

    int chan = m.getChannel() - 1;
    if (chan < 0 || chan >= 16)
        return;

    if (m.isNoteOn() && !isChannelAudible(chan))
        return;

    if (m.isNoteOn())
        fluid_synth_noteon(synth, chan, m.getNoteNumber(), m.getVelocity());
    else if (m.isNoteOff())
        fluid_synth_noteoff(synth, chan, m.getNoteNumber());
    else if (m.isController())
        fluid_synth_cc(synth, chan, m.getControllerNumber(), m.getControllerValue());
    else if (m.isPitchWheel())
        fluid_synth_pitch_bend(synth, chan, m.getPitchWheelValue());
    else if (m.isProgramChange())
    {
        std::cout << "[POG] BE: MIDI: Ch " << (chan + 1) << " -> Program " << m.getProgramChangeNumber() << std::endl;
        fluid_synth_program_change(synth, chan, m.getProgramChangeNumber());
    }

    if (onMidiActivity && (m.isNoteOn() || m.isNoteOff()))
        onMidiActivity(chan, m.getVelocity());
}

// =========================
// PROGRAM CHANGE (NEW)
// =========================
void MidiPlayer::sendProgramChange(int trackNum, int program)
{
    if (!synth)
        return;

    int chan = trackNum - 1;

    if (chan >= 0 && chan < 16)
    {
        std::cout << "[POG] BE: PROGRAM CHANGE - Track: " << trackNum
                  << " Program: " << program << std::endl;

        fluid_synth_program_change(synth, chan, program);
    }
}

// =========================
// VOLUME
// =========================
void MidiPlayer::setMasterVolume(float v)
{
    std::cout << "[POG] BE: Master Volume -> " << v << std::endl;
    masterVolume = v / 127.0f;
}

// =========================
// CC
// =========================
void MidiPlayer::sendRealTimeControlChange(int trackNum, int controller, int value)
{
    if (!synth)
        return;

    int chan = trackNum - 1;

    if (chan >= 0 && chan < 16)
    {
        std::cout << "[POG] BE: MIDI CC SEND - Track: " << trackNum << " CC: " << controller << " Val: " << value << std::endl;
        fluid_synth_cc(synth, chan, controller, value);
    }
}

// =========================
// MUTE / SOLO
// =========================
void MidiPlayer::setChannelMute(int trackIdx, bool mute)
{
    if (trackIdx < 0 || trackIdx >= 16)
        return;

    channelMuted[trackIdx] = mute;

    if (mute && synth)
        fluid_synth_all_sounds_off(synth, trackIdx);
}

void MidiPlayer::setChannelSolo(int trackIdx, bool solo)
{
    if (trackIdx < 0 || trackIdx >= 16)
        return;

    channelSolo[trackIdx] = solo;
}

bool MidiPlayer::isChannelAudible(int channel) const
{
    bool anySolo = false;
    for (int i = 0; i < 16; ++i)
    {
        if (channelSolo[i])
        {
            anySolo = true;
            break;
        }
    }

    if (anySolo)
        return channelSolo[channel];
    return !channelMuted[channel];
}

// =========================
// BANK HELPER
// =========================
int MidiPlayer::getFullBank(int chan)
{
    return (currentBankMSB[chan] << 7) | currentBankLSB[chan];
}