// GuiWorker.h
#pragma once

#include <vector>
#include <string>

#include <SDL3/SDL.h>   // ✅ nutné
#include <fluidsynth.h> // ✅ nutné

struct Track
{
    std::string name;
    float volume;
    float pan;
    bool mute;
    bool solo;
};

class GuiWorker
{
public:
    GuiWorker(fluid_synth_t *synth);
    ~GuiWorker();

    void showWindow();

private:
    void renderTracks();

    fluid_synth_t *synth = nullptr; // ✅ typ už existuje

    std::vector<Track> tracks;

    SDL_Window *window = nullptr;      // ✅ typ už existuje
    SDL_GLContext glContext = nullptr; // ✅ typ už existuje
};