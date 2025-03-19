/*============================================================================*
File: SimulationSettings.h
Desc: Contains the global settings to control various aspects of the 
        simulation.
NOTE: I haven't experimented a whole lot with altering some of these settings.
      I will say, the MAX_GENERATIONS has a huge effect on runtime.
*============================================================================*/

#pragma once

/****************************
*    SIMULATION SETTINGS    *
****************************/
// Globally alter various settings for the simulation
enum SimulationSettings {

    MAX_GENERATIONS = 3000,     // # of generations to simulate
    MAX_POPULATION = 200,       // Forced Population size

    STARTING_BATTERY_RATE = 40, // % of batteries at start (compared to grid size)
    FITNESS_INCREASE = 5,       // Fitness increase
    STARTING_ENERGY = 5,        // Starting energy for each bot
    GENES_PER_BOT = 16,         // How many genese each bot has
    GENE_MUTATION_CHANCE = 5,   // Chance of gene mutation

    MAP_GRID_WIDTH = 10,        // Width of the grid that Robots navigate
    MAP_GRID_HEIGHT = 10,       // Height of the grid that Robots navigate

    GRAPH_HEIGHT_SCALE = 4,     // Changes height ratio. Lower = taller bars
    PROGRESS_UPDATE_RATE = 5    // Progress-update Frequency (0 to disable)
};

/***********************************
 *    Global SimulationController Symbols    *
 **********************************/
 // These are used throughout the program to represent different objects
enum SymbolKeys {
    ROBOT = 'R',

    EMPTY = '0',
    WALL = 'W',
    BATTERY = '*',
    IGNORE = 'x',

    UP = '^',
    DOWN = 'v',
    LEFT = '<',
    RIGHT = '>',
    RANDOM = '?'
};