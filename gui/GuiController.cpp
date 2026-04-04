/**
 * @file GuiController.cpp
 * @brief Implementace GuiController
 * @description Obsahuje metody pro spuštění GuiWorker.
 */

#include "GuiController.h"
#include "GuiWorker.h"

GuiController::GuiController(fluid_synth_t *synth)
{
    // předáme synth do workeru
    worker = std::make_unique<GuiWorker>(synth);
}

GuiController::~GuiController() = default;

void GuiController::run()
{
    worker->showWindow();
}