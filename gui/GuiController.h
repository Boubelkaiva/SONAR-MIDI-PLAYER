/**
 * @file GuiController.h
 * @brief Controller pro GUI modul SONAR MIDI PLAYER
 * @description Orchestruje GuiWorker, který vykresluje okno aplikace.
 */

#pragma once
#include <memory>
#include <fluidsynth.h> // aby mohl přijmout synth

class GuiWorker;

class GuiController
{
public:
    // Konstruktor nyní přijímá fluid_synth_t*
    explicit GuiController(fluid_synth_t *synth);
    ~GuiController();

    void run();

private:
    std::unique_ptr<GuiWorker> worker;
};