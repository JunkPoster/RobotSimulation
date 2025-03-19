/*============================================================================*
File: SimulationMenu.h
Desc: Contains the main menu for the SimulationController. This is where the
      user can run the simulation, view stats, or quit the program.
*============================================================================*/

#pragma once

#include "SimulationController.h"
#include "../../utilities/ValidateInput.h"

class SimulationMenu {
    Logger classLog;
    SimulationController sim;
    ValidateInput getInput;
    bool quitFlag = false;

public:
    SimulationMenu();
    ~SimulationMenu();
    void mainMenu();
    void resetSim();
    void runSim();
    void viewStats() const;
    void printMainMenu() const;
    void printIntro() const;
};
