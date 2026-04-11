/*
  ==============================================================================
    FILE: MidiPlayer.cpp
    PROJECT: SONAR MIDI PLAYER
    DESCRIPTION: Audio Engine with real-time TSF Index mapping.
    UPDATED: Přidány detailní debug logy pro lokalizaci pádů.
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
    : isPlaying(false), currentSampleRate(44100.0), masterVolume(1.0f),
      g_tinyfont(nullptr), playheadSeconds(0.0), currentMode(MidiMode::GM)
{
    mapper = std::make_unique<MidiMapper>(nullptr);

    for (int i = 0; i < 16; ++i)
    {
        currentBankMSB[i] = (i == 9) ? 128 : 0;
        currentBankLSB[i] = 0;
    }
    std::cout << "[MidiPlayer] Konstruktor dokončen." << std::endl;
}

MidiPlayer::~MidiPlayer()
{
    if (g_tinyfont)
        tsf_close(g_tinyfont);
}

void MidiPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    renderBuffer.setSize(1, samplesPerBlockExpected * 2);

    if (g_tinyfont)
        tsf_set_output(g_tinyfont, TSF_STEREO_INTERLEAVED, (float)sampleRate, 0.0f);
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

    double blockDuration = (double)bufferToFill.numSamples / currentSampleRate;
    double nextPlayheadTime = playheadSeconds + blockDuration;

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

    renderBuffer.clear();
    int samplesToRender = bufferToFill.numSamples;
    tsf_render_float(g_tinyfont, renderBuffer.getWritePointer(0), samplesToRender, 0);

    const float *tsfOut = renderBuffer.getReadPointer(0);
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
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
    std::cout << "\n[CHECKPOINT 0] Start loadMidiFile: " << midiFile.getFileName() << std::endl;
    stop();

    std::thread([this, midiFile]() mutable
                {
                    // --- KROK 1: MIDI LOAD ---
                    juce::MidiFile mf;
                    auto is = std::unique_ptr<juce::InputStream>(midiFile.createInputStream());
                    if (is == nullptr || !mf.readFrom(*is))
                        return;
                    mf.convertTimestampTicksToSeconds();

                    // --- KROK 2: SOUNDFONT CHECKPOINT ---
                    // Čekáme, dokud g_tinyfont není validní (pokud se zrovna načítá jinde)
                    int retryCount = 0;
                    while (g_tinyfont == nullptr && retryCount < 20)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        retryCount++;
                    }

                    if (g_tinyfont != nullptr && mapper != nullptr)
                    {
                        // AKTUALIZACE MAPPERU - Teď máme jistotu, že g_tinyfont existuje
                        mapper->updateTSFInstance(g_tinyfont);

                        // BREAKPOINT LOG: Ověříme, že mapper není slepý
                        std::cout << "[CHECKPOINT 1] Mapper připraven. Instance TSF: " << g_tinyfont << std::endl;

                        // --- KROK 3: ANALÝZA ---
                        MidiAnalyzer analyzer;
                        auto results = analyzer.analyzeFile(midiFile, mapper.get());

                        // KONTROLA VÝSLEDKŮ: Pokud jsou v results samé -1, víme, že je problém v mapperu
                        bool analysisValid = false;
                        for (auto &r : results)
                            if (r.tsfIndex >= 0)
                            {
                                analysisValid = true;
                                break;
                            }

                        if (analysisValid)
                        {
                            std::cout << "[CHECKPOINT 2] Analýza úspěšná (indexy nalezeny)." << std::endl;
                        }
                        else
                        {
                            std::cout << "[CHECKPOINT 2] VAROVÁNÍ: Analýza vrátila prázdné indexy (vše -1)." << std::endl;
                        }

                        // --- KROK 4: APLIKACE (Musí proběhnout v tomto pořadí!) ---
                        applyAnalysisResults(results);
                    }

                    // --- KROK 5: FINÁLNÍ INSTANCE SEKVENCE ---
                    juce::MidiMessageSequence tempSequence;
                    for (int i = 0; i < mf.getNumTracks(); ++i)
                        if (auto *track = mf.getTrack(i))
                            tempSequence.addSequence(*track, 0, 0, mf.getLastTimestamp());

                    tempSequence.updateMatchedPairs();

                    // Odevzdání hotové práce hlavnímu vláknu
                    juce::MessageManager::callAsync([this, tempSeq = std::move(tempSequence)]() mutable
                                                    {
            this->midiSequence = std::move(tempSeq);
            std::cout << "[CHECKPOINT 3] Hotovo. Sekvence předána. Připraveno k přehrávání.\n" << std::endl; });
                })
        .detach();
}

void MidiPlayer::applyAnalysisResults(const std::vector<TrackData> &results)
{
    if (!g_tinyfont)
    {
        std::cout << "[MidiPlayer] Error: applyAnalysisResults voláno bez SoundFontu!" << std::endl;
        return;
    }

    std::cout << "[MidiPlayer] Nastavuji " << results.size() << " stop..." << std::endl;
    for (const auto &data : results)
    {
        if (data.tsfIndex >= 0)
        {
            tsf_channel_set_presetindex(g_tinyfont, data.channel - 1, data.tsfIndex);
            currentBankMSB[data.channel - 1] = data.bankMSB;
            currentBankLSB[data.channel - 1] = data.bankLSB;
            std::cout << "[MidiPlayer] Kanál " << data.channel << " -> Preset " << data.tsfIndex << std::endl;
        }
    }
}

void MidiPlayer::loadSoundFont(const juce::File &sf2File)
{
    isPlaying = false;
    std::cout << "[MidiPlayer] Načítám SoundFont: " << sf2File.getFullPathName() << std::endl;

    if (g_tinyfont)
        tsf_close(g_tinyfont);

    g_tinyfont = tsf_load_filename(sf2File.getFullPathName().toRawUTF8());

    if (g_tinyfont)
    {
        tsf_set_output(g_tinyfont, TSF_STEREO_INTERLEAVED, (float)currentSampleRate, 0.0f);
        if (mapper)
            mapper->updateTSFInstance(g_tinyfont);

        std::cout << "[MidiPlayer] SoundFont úspěšně naložen." << std::endl;
    }
    else
    {
        std::cout << "[MidiPlayer] ERROR: TSF nemohl načíst font!" << std::endl;
    }
}

void MidiPlayer::play()
{
    if (g_tinyfont)
    {
        std::cout << "[MidiPlayer] Play..." << std::endl;
        isPlaying = true;
    }
}

void MidiPlayer::stop()
{
    isPlaying = false;
    playheadSeconds = 0.0;
    if (g_tinyfont)
    {
        tsf_reset(g_tinyfont);
        std::cout << "[MidiPlayer] Stop & Reset." << std::endl;
    }
}

void MidiPlayer::pause() { isPlaying = false; }

void MidiPlayer::setMasterVolume(float v)
{
    masterVolume = v / 127.0f;
}