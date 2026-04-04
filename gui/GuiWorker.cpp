// GuiWorker.cpp
#include "GuiWorker.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <stdexcept>
#include <string>

GuiWorker::GuiWorker(fluid_synth_t *synth)
    : synth(synth)
{
    if (!synth)
    {
        throw std::runtime_error("Synth is null!");
    }

    // Inicializace 16 tracků
    for (int i = 0; i < 16; ++i)
    {
        tracks.push_back({"Track " + std::to_string(i + 1), 0.8f, 0.0f, false, false});
    }

    // --- SDL init ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    // --- OpenGL setup ---
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        "SONAR MIDI PLAYER",
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        throw std::runtime_error(SDL_GetError());
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    // --- ImGui init ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");
}

GuiWorker::~GuiWorker()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (glContext)
        SDL_GL_DestroyContext(glContext);

    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

void GuiWorker::renderTracks()
{
    ImGui::Begin("Tracks");

    int i = 0;
    for (auto &t : tracks)
    {
        ImGui::PushID(i);

        int channel = i; // 🎯 MIDI kanál 0–15

        ImGui::Text("%s", t.name.c_str());

        if (ImGui::SliderFloat("Volume", &t.volume, 0.0f, 1.0f))
        {
            fluid_synth_cc(synth, channel, 7, (int)(t.volume * 127)); // CC7 volume
        }

        if (ImGui::SliderFloat("Pan", &t.pan, -1.0f, 1.0f))
        {
            int pan = (int)((t.pan + 1.0f) * 63.5f); // 0–127
            fluid_synth_cc(synth, channel, 10, pan); // CC10 pan
        }

        if (ImGui::Checkbox("Mute", &t.mute))
        {
            if (t.mute)
                fluid_synth_cc(synth, channel, 7, 0);
            else
                fluid_synth_cc(synth, channel, 7, (int)(t.volume * 127));
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Solo", &t.solo))
        {
            // 🎯 jednoduchá solo logika
            for (int j = 0; j < 16; ++j)
            {
                if (j != channel)
                {
                    fluid_synth_cc(synth, j, 7, t.solo ? 0 : 100);
                }
            }
        }

        ImGui::Separator();

        ImGui::PopID();
        i++;
    }

    ImGui::End();
}

void GuiWorker::showWindow()
{
    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        renderTracks();

        ImGui::Render();

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
}