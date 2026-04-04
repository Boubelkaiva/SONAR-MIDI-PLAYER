/**
 * @file AppController.cpp
 * @brief Implementace hlavního controlleru
 * @description Obsahuje metody pro spuštění GUI modulu.
 */

#include "AppController.h"
#include "gui/GuiController.h"

AppController::AppController()
{
    gui = std::make_unique<GuiController>();
}

AppController::~AppController() = default;

void AppController::run()
{
    gui->run();
}