/*============================================================================*
File: StatsRobots.h
Desc: Contains the RobotStats struct, which is used to store the stats of each
        Robot in the SimulationController.
*============================================================================*/

#pragma once

/*    RobotStats Struct    */
struct RobotStats {
    int fitness = 0;
    int turns = 0;
    int rounds = 0;
    int generation = 0;
    int mutations = 0;

    double getAvgFitness() const;
    double getAvgTurns() const;
    

    RobotStats() = default;
    RobotStats(const RobotStats& copy);
    RobotStats& operator=(const RobotStats& copy);
};