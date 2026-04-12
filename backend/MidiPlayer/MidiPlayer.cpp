/*
  ==============================================================================
    FILE: MidiPlayer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine with real-time TSF Index mapping.
    UPDATED: Fixed Mute/Solo logic and real-time CC response.
  ==============================================================================
*/

#define TSF_IMPLEMENTATION
#include "tsf.h"
#include "MidiPlayer.h"
#include "../MidiAnalyzer/MidiAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <thread>

MidiPlayer::MidiPlayer()
    : isPlaying(false), currentSampleRate(48000.0), masterVolume(1.0f),
      g_tinyfont(nullptr), playheadSeconds(0.0), currentMode(MidiMode::GM)
{
    mapper = std::make_unique<MidiMapper>(nullptr);

    for (int i = 0; i < 16; ++i)
    {
        currentBankMSB[i] = (i == 9) ? 128 : 0;
        currentBankLSB[i] = 0;
        channelMuted[i] = false; // Inicializace Mute
        channelSolo[i] = false;  // Inicializace Solo
    }
    std::cout << "[MidiPlayer] Konstruktor dokončen. Výchozí frekvence: 48kHz." << std::endl;
}

MidiPlayer::~MidiPlayer()
{
    if (g_tinyfont)
        tsf_close(g_tinyfont);
}

// Pomocná funkce pro vyhodnocení, zda má kanál hrát
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

void MidiPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    renderBuffer.setSize(1, samplesPerBlockExpected * 2);

    if (g_tinyfont)
    {
        std::cout << "[AUDIO] Hardware chce: " << sampleRate << " Hz. Přizpůsobuji TSF." << std::endl;
        tsf_set_output(g_tinyfont, TSF_STEREO_INTERLEAVED, (float)sampleRate, 0.0f);
    }
}

void MidiPlayer::releaseResources()
{
    renderBuffer.setSize(0, 0);
}

void MidiPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (!isPlaying || g_tinyfont == nullptr || currentSampleRate <= 0)
        return;

    int samplesToRender = bufferToFill.numSamples;
    double blockDuration = (double)samplesToRender / currentSampleRate;
    double nextPlayheadTime = playheadSeconds + blockDuration;

    // --- MIDI EVENTY ZE SEKVENCE ---
    for (int i = 0; i < midiSequence.getNumEvents(); ++i)
    {
        auto *event = midiSequence.getEventPointer(i);
        double eventTime = event->message.getTimeStamp();

        if (eventTime >= playheadSeconds && eventTime < nextPlayheadTime)
        {
            auto &msg = event->message;
            int chan = msg.getChannel() - 1;
            if (chan < 0 || chan > 15)
                continue;

            // Filtrování NoteOn podle Mute/Solo
            if (msg.isNoteOn() && !isChannelAudible(chan))
                continue;

            if (msg.isController())
            {
                int cc = msg.getControllerNumber();
                int val = msg.getControllerValue();
                if (cc == 0)
                    currentBankMSB[chan] = val;
                else if (cc == 32)
                    currentBankLSB[chan] = val;
                else
                    tsf_channel_midi_control(g_tinyfont, chan, cc, val);
            }
            else if (msg.isProgramChange())
            {
                int prog = msg.getProgramChangeNumber();
                if (mapper)
                {
                    int idx = mapper->findDeepPresetIndex(currentBankMSB[chan], prog, chan, currentMode);
                    if (idx >= 0)
                        tsf_channel_set_presetindex(g_tinyfont, chan, idx);
                }
            }
            else if (msg.isNoteOn())
                tsf_channel_note_on(g_tinyfont, chan, msg.getNoteNumber(), msg.getFloatVelocity());
            else if (msg.isNoteOff())
                tsf_channel_note_off(g_tinyfont, chan, msg.getNoteNumber());
            else if (msg.isPitchWheel())
                tsf_channel_set_pitchwheel(g_tinyfont, chan, msg.getPitchWheelValue());
        }
    }

    // --- AUDIO RENDERING ---
    if (renderBuffer.getNumSamples() < samplesToRender * 2)
        renderBuffer.setSize(1, samplesToRender * 2);

    renderBuffer.clear();
    tsf_render_float(g_tinyfont, renderBuffer.getWritePointer(0), samplesToRender, 0);

    const float *tsfOut = renderBuffer.getReadPointer(0);
    int numHardwareChannels = bufferToFill.buffer->getNumChannels();

    for (int channel = 0; channel < numHardwareChannels; ++channel)
    {
        float *deviceOut = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        for (int i = 0; i < samplesToRender; ++i)
        {
            deviceOut[i] = tsfOut[i * 2 + (channel % 2)] * masterVolume;
        }
    }

    playheadSeconds = nextPlayheadTime;
    if (playheadSeconds >= midiSequence.getEndTime())
        stop();
}

void MidiPlayer::loadMidiFile(const juce::File &midiFile)
{
    stop();
    std::thread([this, midiFile]() mutable
                {
        juce::MidiFile mf;
        auto is = std::unique_ptr<juce::InputStream>(midiFile.createInputStream());
        if (is == nullptr || !mf.readFrom(*is)) return;
        mf.convertTimestampTicksToSeconds();

        int retryCount = 0;
        while (g_tinyfont == nullptr && retryCount < 20)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            retryCount++;
        }

        if (g_tinyfont != nullptr && mapper != nullptr)
        {
            mapper->updateTSFInstance(g_tinyfont);
            MidiAnalyzer analyzer;
            auto results = analyzer.analyzeFile(midiFile, mapper.get());
            applyAnalysisResults(results);
        }

        juce::MidiMessageSequence tempSequence;
        for (int i = 0; i < mf.getNumTracks(); ++i)
            if (auto *track = mf.getTrack(i))
                tempSequence.addSequence(*track, 0, 0, mf.getLastTimestamp());

        tempSequence.updateMatchedPairs();

        juce::MessageManager::callAsync([this, tempSeq = std::move(tempSequence)]() mutable
        {
            this->midiSequence = std::move(tempSeq);
        }); })
        .detach();
}

void MidiPlayer::applyAnalysisResults(const std::vector<TrackData> &results)
{
    if (!g_tinyfont)
        return;
    for (const auto &data : results)
    {
        if (data.tsfIndex >= 0)
        {
            tsf_channel_set_presetindex(g_tinyfont, data.channel - 1, data.tsfIndex);
            currentBankMSB[data.channel - 1] = data.bankMSB;
            currentBankLSB[data.channel - 1] = data.bankLSB;
        }
    }
}

void MidiPlayer::loadSoundFont(const juce::File &sf2File)
{
    isPlaying = false;
    if (g_tinyfont)
        tsf_close(g_tinyfont);
    g_tinyfont = tsf_load_filename(sf2File.getFullPathName().toRawUTF8());
    if (g_tinyfont)
    {
        tsf_set_output(g_tinyfont, TSF_STEREO_INTERLEAVED, (float)currentSampleRate, 0.0f);
        if (mapper)
            mapper->updateTSFInstance(g_tinyfont);
    }
}

void MidiPlayer::play()
{
    if (g_tinyfont)
        isPlaying = true;
}
void MidiPlayer::stop()
{
    isPlaying = false;
    playheadSeconds = 0.0;
    if (g_tinyfont)
        tsf_reset(g_tinyfont);
}
void MidiPlayer::pause() { isPlaying = false; }
void MidiPlayer::setMasterVolume(float v) { masterVolume = v / 127.0f; }

// --- SETTERY PRO MUTE / SOLO ---
void MidiPlayer::setChannelMute(int trackIdx, bool mute)
{
    if (trackIdx >= 0 && trackIdx < 16)
    {
        channelMuted[trackIdx] = mute;

        // Pokud je Mute aktivní, okamžitě "zbijeme" všechny hrající hlasy
        if (mute && g_tinyfont)
        {
            tsf_channel_sounds_off_all(g_tinyfont, trackIdx);
        }
    }
}

void MidiPlayer::setChannelSolo(int trackIdx, bool solo)
{
    if (trackIdx >= 0 && trackIdx < 16)
    {
        channelSolo[trackIdx] = solo;

        if (g_tinyfont)
        {
            // Při Solo musíme utnout všechny kanály, které teď mají mlčet
            for (int i = 0; i < 16; ++i)
            {
                if (!isChannelAudible(i))
                {
                    tsf_channel_sounds_off_all(g_tinyfont, i);
                }
            }
        }
    }
}

// --- REAL-TIME ZMĚNY ---
void MidiPlayer::sendRealTimeControlChange(int trackNum, int controller, int value)
{
    if (g_tinyfont != nullptr)
    {
        int chan = trackNum - 1;
        if (chan >= 0 && chan < 16)
        {
            tsf_channel_midi_control(g_tinyfont, chan, controller, value);
            std::cout << "[LIVE MIDI] Track: " << trackNum << " | CC: " << controller << " | Val: " << value << std::endl;
        }
    }
}