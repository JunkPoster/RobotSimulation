// This is a deprecated file, but it's what I used to debug the simulation.

#pragma once
/**********************
 *    SimInspector    *
 *********************/
 /* Made this to inspect the performance of each aspect of the simulation to
 make sure they're running correctly. Helped me fix a critical bug involving
 gene splicing. */
class SimInspector : public Simulation {
    Logger classLog;

    bool quitFlag = false;

    static const int NUM_OPTIONS = 2;

public:
    SimInspector() : classLog(FUNC) {}
    ~SimInspector() {}

    void inspectMenu() {
        while (!quitFlag) {

            string userInput = "";
            cout << COLOR("\n[INSPECTOR] Choose an option below:\n", CYAN);
            printMenu();

            cout << "Enter Option here: ";
            getline(cin, userInput);

            switch (toupper(userInput[0])) {
            case 'A':
                inspectPopulation();
                break;
            case 'B':
                inspectBreeding();
                break;
            case 'Q':
                quitFlag = true;
                break;
            default:
                cout << "\nInvalid Option!\n";
                continue;
            }
        }
    }

    void resetSim() {
        clearBots();
        currGen = 0;
    }

    void inspectPopulation() {

        resetSim();

        currGen++;
        initializePopulation();
        generationCycle();
        cout << COLOR("\n[Unsorted]", YELLOW, BRIGHT) << " ";
        printPopStats();
        if (!getInput.pressEnter()) return;

        sortBots();
        cout << COLOR("\n[Sorted]", YELLOW, BRIGHT) << " ";
        printPopStats();
        if (!getInput.pressEnter()) return;

        purgePopulation();
        cout << COLOR("\n[Purged]", YELLOW, BRIGHT) << " ";
        printPopStats();
        if (!getInput.pressEnter()) return;

        currGen++;
        breedPopulation();
        cout << COLOR("\n[Bred]", YELLOW, BRIGHT) << " ";
        printPopStats();

        if (!getInput.pressEnter()) return;
    }

    void inspectBreeding() {
        Robot p1;
        Robot p2;
        Robot c1 = p1 + p2;
        Robot c2 = p1 + p2;
        compareGenes(p1.getGenes(), p2.getGenes(), c1.getGenes(), c2.getGenes());

        if (!getInput.pressEnter()) quitFlag = true;
    }

    // Print Detailed Stats on a bot
    void printBotStats(size_t i) {
        RobotStats curStats = bots[i]->bot.getStats();

        double avgFit = curStats.getAvgFitness();
        double avgTrn = curStats.getAvgTurns();
        int curFit = curStats.fitness;
        int curTrn = curStats.turns;
        int curRnd = curStats.rounds;
        int curMut = curStats.mutations;
        int gen = curStats.generation;

        double calFit;
        if (curRnd == 0) calFit = 0;
        else calFit = (double)curFit / (double)curRnd;

        cout << align((COLOR(i, WHITE) + ". "), 'R', 5)
            << align(("[" + COLOR(gen, YELLOW)) + "]", 'L', 5)
            << align(("Fit: " + COLOR(curFit, CYAN)), 'L', 8)
            << align(("Trn: " + COLOR(curTrn, CYAN, BRIGHT)), 'L', 8)
            << align(("Rnd: " + COLOR(curRnd, BLUE, BRIGHT)), 'L', 8)
            << align(("Mut: " + COLOR(curMut, RED, BRIGHT)), 'L', 8)
            << align(("AvgFit: " + COLOR(avgFit, GREEN)), 'L', 12)
            << align(("(" + COLOR((calFit), GREEN, BRIGHT) + ")"), 'L', 5)
            << align(("AvgTrn: " + COLOR(avgTrn, GREEN)), 'L', 12);
    }

    // Print the top-performing bots' stats
    void printTopBots(size_t n) {
        cout << "Round: " << currGen << "\n";
        for (size_t i = 0; i < n; i++) {
            printBotStats(i);
            cout << "\n";
        }
        cout << "\n";
    }

    void printPopStats() {
        cout << "Population: " << COLOR(bots.size(), YELLOW) << "\n";

        cout << "  Format: (" << COLOR("Generation #", BLACK, BRIGHT) << "."
            << COLOR("AvgFitness", GREEN, BRIGHT) << ") \n";

        for (size_t i = 0; i < bots.size(); i++) {
            const int COLUMNS = 10;

            RobotStats curStats = bots[i]->bot.getStats();
            double fit = curStats.getAvgFitness();
            int gen = curStats.generation;

            cout << COLOR(((gen < 10) ? " G" : "G")
                + to_string(gen), BLACK, BRIGHT) << ".";

            if ((i + 1) > (MAX_POPULATION / 2)) cout << COLOR(fit, RED, BRIGHT);
            else cout << COLOR(fit, GREEN, BRIGHT);
            cout << ((fit < 10) ? " " : "");

            if ((i + 1) % COLUMNS == 0) cout << "\n";
            else cout << "|";
        }
        cout << "\n";
    }

    // Displays a single gene in color
    static void displayGene(const Gene& gene, Colors color,
        Colors mod = RESET) {

        for (int i = 0; i < gene.NUM_GENE_CODES; i++) {
            cout << COLOR(gene.gene[i], color, mod);
        }
    }

    // Compares the genes of two parents and their children and then 
    // displays them in color
    static void compareGenes(const GeneManager& par1, const GeneManager& par2,
        const GeneManager& c1, const GeneManager& c2) {
        vector<Gene> p1 = par1.getAllGenes();
        vector<Gene> p2 = par2.getAllGenes();
        vector<Gene> child1 = c1.getAllGenes();
        vector<Gene> child2 = c2.getAllGenes();

        cout << COLOR("\nParent vs Child Genes:\n", YELLOW);
        cout << "   (Purple = " << COLOR("Mutated", MAGENTA, BRIGHT) << ")\n";

        cout << COLOR("Parent 1: ", GREEN, BRIGHT);
        for (const auto& gene : p1) {
            for (size_t i = 0; i < gene.gene.size(); ++i) {
                cout << COLOR(gene.gene[i], GREEN, BRIGHT);
            }
            cout << "|";
        }
        cout << COLOR("\nParent 2: ", CYAN);
        for (const auto& gene : p2) {
            for (size_t i = 0; i < gene.gene.size(); ++i) {
                cout << COLOR(gene.gene[i], CYAN);
            }
            cout << "|";
        }

        cout << "\nChild  1: ";
        for (size_t i = 0; i < p1.size(); i++) {
            Gene curGene = child1[i];
            if (curGene == p1[i]) displayGene(curGene, GREEN, BRIGHT);
            else if (curGene == p2[i]) displayGene(curGene, CYAN);
            else displayGene(curGene, MAGENTA, BRIGHT);
            cout << "|";
        }

        cout << "\nChild  2: ";
        for (size_t i = 0; i < p1.size(); i++) {
            Gene curGene = child2[i];
            if (curGene == p1[i]) displayGene(curGene, GREEN, BRIGHT);
            else if (curGene == p2[i]) displayGene(curGene, CYAN);
            else displayGene(curGene, MAGENTA, BRIGHT);
            cout << "|";
        }

        cout << "\n\n";
    }

    void printMenu() {
        cout << "[" << COLOR('A', GREEN, BRIGHT) << "] "
            << "Inspect Purge & Breeding Mechanics" << endl;
        cout << "[" << COLOR('B', GREEN, BRIGHT) << "] "
            << "Inspect Breeding Effects on Genes" << endl;
        cout << "[" << COLOR('Q', RED, BRIGHT) << "] "
            << "Quit to Main Menu" << endl;
    }
};
