/**
 * @file AppController.h
 * @brief Hlavní controller aplikace
 * @description Orchestruje moduly aplikace (GUI, MIDI, audio) pro main.cpp.
 */

#pragma once
#include <memory>

class GuiController;

class AppController
{
public:
    AppController();
    ~AppController();

    void run();

private:
    std::unique_ptr<GuiController> gui;
};