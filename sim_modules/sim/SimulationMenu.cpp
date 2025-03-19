/*============================================================================*
File: SimulationMenu.cpp
Desc: Contains the main menu for the SimulationController. This is where the
      user can run the simulation, view stats, or quit the program.
*============================================================================*/

#include "SimulationMenu.h"

SimulationMenu::SimulationMenu() : classLog(FUNC) {}
SimulationMenu::~SimulationMenu() {}

// Extremely rudimentary Main menu for the SimulationController
void SimulationMenu::mainMenu() {
    Logger log(FUNC);

    printIntro();

    // Loop main menu until the user quits
    while (!quitFlag) {

        string userInput = "";
        cout << COLOR("\nChoose an option below:\n", CYAN);
        printMainMenu();

        cout << "Enter Option here: ";
        getline(cin, userInput);

        switch (toupper(userInput[0])) {
        case 'A':   // Run SimulationController
            runSim();
            if (!getInput.pressEnter()) return;
            break;
        case 'B':   // View Stats
            if (sim.getCurrGen() == 0) {
                cout << "\nYou must run a Simulation first!\n";
                continue;
            }
            viewStats();
            if (!getInput.pressEnter()) return;
            break;
        case 'Q':   // Quit
            quitFlag = true;
            break;
        default:
            cout << "\nInvalid Option!\n";
            continue;
        }
    }
}

// Completely re-instantiates the SimulationController Object
void SimulationMenu::resetSim() {
    Logger log(FUNC);
    sim = SimulationController();
}

// Run a new SimulationController. If the SimulationController has already been run, reset it.
void SimulationMenu::runSim() {
    Logger log(FUNC);

    if (sim.getCurrGen() > 0) {
        cout << "Resetting Simulation...\n";
        resetSim();
    }
    cout << "\nRunning Simulation (" + to_string(MAX_GENERATIONS)
        + " Generations) ...\n";
    sim.beginSim();
}

// View more detailed Generational stats from latest SimulationController run
void SimulationMenu::viewStats() const {
    Logger log(FUNC);

    if (sim.getCurrGen() == 0) {
        cout << "\nYou must run a Simulation first!\n";
        return;
    }
    sim.stats.printGenerationalStats();
}

void SimulationMenu::printMainMenu() const {
    cout << "[" << COLOR('A', GREEN, BRIGHT) << "] "
        << "Run a New Simulation" << endl;
    cout << "[" << COLOR('B', GREEN, BRIGHT) << "] "
        << "View Each Generation's Stats" << endl;
    cout << "[" << COLOR('Q', RED, BRIGHT) << "] "
        << "Quit" << endl;
}

// Finally revisited my WrapText() function to make it more versatile. 
// It's not thoroughly tested, but it seems to work well here.
void SimulationMenu::printIntro() const {
    FormatText introFt(2);
    const int WIDTH = 70;

    string str = COLOR("A Simulation of Evolution ", GREEN, BRIGHT) +
        "\nThis program will use a population of Robots to simulate "
        "the effects of evolution over time.\n\n";
    str += COLOR("The Robots: ", BLUE, BRIGHT) +
        "Each Robot will be given a set of Genes that "
        "will dictate how they interact with their environment. "
        "The Robots will then compete by navigating a random map to "
        "see who can collect the most batteries.\n\n";
    str += COLOR("The Simulation: ", BLUE, BRIGHT) +
        "At the end of each round, the Bots will be sorted by how "
        "successful they were at harvesting energy that round (Fitness). "
        "The bottom 50% will be eliminated from the simulation and "
        "the top 50% will be allowed to breed, combining their superior "
        "genes. This process will repeat until the maximum number of "
        "generations has been reached.\n\n";
    str += COLOR("The Statistics: ", BLUE, BRIGHT) +
        "At the end of the simulation, you will be able to view the "
        "trends for each Generation's average fitness scores.";

    cout << "\n" << introFt.wrapText(str, 'L', WIDTH) << "\n\n";
}
