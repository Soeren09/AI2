//
// Created by soeren on 4/16/20.
//

#ifndef AI_PARAMETERS_CPP
#define AI_PARAMETERS_CPP

#include <array>
const int VERBOSE = 0;

// Local map position of globus and stars
const std::array<int, 8> POS_GLOBUS         = {1, 9, 14, 22, 27, 35, 40, 48 };
const std::array<int,3> POS_ENEMY_GLOBUS    = {14, 27, 40};
const std::array<int,5> POS_SAFE_GLOBUS     = {1, 9, 22, 35, 48};
const std::array<int, 8> POS_STAR           = {6, 12, 19, 25, 32, 38, 45, 51 };
const int N_STAR = 8;
const int N_GLOBUS = 8;

// Key map position
const int HOME_POSITION = 0;
const int START_POSITION = 1;
const int GOAL_POSITION = 57;
const int UNREACHABLE_POSITION = -1;    // Defined as a home or goal area location

const int GOAL_AREA_START = 52;
const int POS_LAST_GLOBUS = 48;
const int POS_LAST_STAR = 51;


// Die Value
const int DIE_SIX = 6;
const int DIE_GLOBUS = 5;
const int DIE_STAR = 3;

//
const int N_PLAYERS = 4;
const int N_ENEMIES = 3;
const int COLLISION_TYPE_NORMAL = 0;
const int COLLISION_TYPE_GLOBUS = 1;


// array containing piece position of each opponent.
typedef std::array<int, 4*(N_ENEMIES)> enemyPiecePos ;


#endif //AI_PARAMETERS_CPP
