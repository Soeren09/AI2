//
// Created by soeren on 5/3/20.
//

#ifndef AI_ACTORCRITICPARAMETERS_H
#define AI_ACTORCRITICPARAMETERS_H

#include <array>

// Actor Critic Parameters
const int N_ACTIONS = 4;
const int ACTION_1 = 0; // Move Piece idx 0
const int ACTION_2 = 1; // Move Piece idx 1
const int ACTION_3 = 2; // Move Piece idx 2
const int ACTION_4 = 3; // Move Piece idx 3

const int N_STATES = 5;
const int STATE_GOAL      = 0; // Piece located at goal
const int STATE_SAFE      = 1; // Piece located at safe zone
const int STATE_NEUTRAL   = 2; // Piece located at normal board position
const int STATE_HOSTILE   = 3; // Piece located at hostile zone -> an enemy player is able to reach the same position
const int STATE_HOME      = 4; // Piece located at home position

// Reward table - transition table
const double REWARD_ENTER_GOAL     = 2;
const double REWARD_SAFE_ZONE      = 1;
const double REWARD_NEUTRAL_ZONE   = 0;
const double REWARD_HOSTILE_ZONE   =-1;
const double REWARD_KILLED         =-2;

const int REWARD_ILLEGAL_MOVE   =-3;
const int REWARD_NA             = 0;
// A transition is indexed as the row being the current State and the coloumn the next state
const std::array<const std::array<double, N_STATES> , N_STATES> REWARD_TABLE = {{
     {REWARD_NA,REWARD_NA, REWARD_NA, REWARD_NA,

      },                                        // Moving from STATE_GOAL to x
     {REWARD_ENTER_GOAL, REWARD_SAFE_ZONE, REWARD_NEUTRAL_ZONE, REWARD_HOSTILE_ZONE, REWARD_NA},     // Moving from STATE_SAFE to x
     {REWARD_ENTER_GOAL, REWARD_SAFE_ZONE, REWARD_NEUTRAL_ZONE, REWARD_HOSTILE_ZONE, REWARD_KILLED}, // Moving from STATE_NEUTRAL to x
     {REWARD_ENTER_GOAL, REWARD_SAFE_ZONE, REWARD_NEUTRAL_ZONE, REWARD_HOSTILE_ZONE, REWARD_KILLED}, // Moving from STATE_HOSTILE to x
     {REWARD_NA, REWARD_SAFE_ZONE, REWARD_NA, REWARD_NA, REWARD_NA}}                                 // Moving from STATE_HOME to x
};


//const int REWARD_TYPE_ENTER_GOAL     = 0;
//const int REWARD_TYPE_SAFE_ZONE      = 1;
//const int REWARD_TYPE_NEUTRAL_ZONE   = 2;
//const int REWARD_TYPE_HOSTILE_ZONE   = 3;
//const int REWARD_TYPE_KILLED         = 4;
//const int REWARD_TYPE_NA             = 5;
//
// // A transition is indexed as the row being the current State and the coloumn the next state
//const std::array<const std::array<int, N_STATES> , N_STATES> REWARD_TYPE_TABLE = {{
//     {REWARD_TYPE_NA,REWARD_TYPE_NA, REWARD_TYPE_NA, REWARD_TYPE_NA, REWARD_TYPE_NA},                                        // Moving from STATE_GOAL to x
//     {REWARD_TYPE_ENTER_GOAL, REWARD_TYPE_SAFE_ZONE, REWARD_TYPE_NEUTRAL_ZONE, REWARD_TYPE_HOSTILE_ZONE, REWARD_TYPE_NA},     // Moving from STATE_SAFE to x
//     {REWARD_TYPE_ENTER_GOAL, REWARD_TYPE_SAFE_ZONE, REWARD_TYPE_NEUTRAL_ZONE, REWARD_TYPE_HOSTILE_ZONE, REWARD_TYPE_KILLED}, // Moving from STATE_NEUTRAL to x
//     {REWARD_TYPE_ENTER_GOAL, REWARD_TYPE_SAFE_ZONE, REWARD_TYPE_NEUTRAL_ZONE, REWARD_TYPE_HOSTILE_ZONE, REWARD_TYPE_KILLED}, // Moving from STATE_HOSTILE to x
//     {REWARD_TYPE_NA, REWARD_TYPE_SAFE_ZONE, REWARD_TYPE_NA, REWARD_TYPE_NA, REWARD_TYPE_NA}}                                 // Moving from STATE_HOME to x
//};



// Type def
typedef std::array<int, 4> actorCriticPieceState;

#endif //AI_ACTORCRITICPARAMETERS_H
