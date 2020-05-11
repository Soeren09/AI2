//
// Created by soeren on 4/29/20.
//
#ifndef AI_PLAYERBASE_H
#define AI_PLAYERBASE_H

#include <array>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include "Parameters.h"
#include <iostream>

class PlayerBase {
protected:
    int PlayerIdx;
    std::array<int, 4> PiecePositions;
    std::array<int, 4> MovablePieceIdx;
    int nMovable;
    int ready;

public:
    PlayerBase(){};
/**
 * PLayer constructor.
 * @param playerIdx Global player idx number
 * @param enemyIdx Global index number of enemies
 * @param piecePosition Array of piece position: 0: Home, 1-51: Board zone, 52-56: Goal entrance, 57: Goal position
*/
    PlayerBase(int playerIdx, const std::array<int, 4> &piecePositions) : PlayerIdx(
            playerIdx), PiecePositions(piecePositions) {
        nMovable = 0;
        ready = 1;
        srand(time(0));  // Initialize random number generator.

    }
    ~PlayerBase(){}

/**
 * Reset the piece position to the homeposition
 */
    void Reset(){
        PiecePositions[0] = HOME_POSITION;
        PiecePositions[1] = HOME_POSITION;
        PiecePositions[2] = HOME_POSITION;
        PiecePositions[3] = HOME_POSITION;
        ready = 1;
    }

/**
 * Set ready flag.
 * @param in =1 if the player is ready to play. = 0 when the player has played
 */
    void GamePlayed(){
        ready = 0;
    }

/**
 * Get ready flag.
 * @param in =1 if the player is ready to play. = 0 when the player has played
 */
    int GetReady(){
        return ready;
    }


/** Function to make decision about what piece to move.
 * The method get two empty int from the game class which the function need to fill out.
 * @param movePieceIdx
 * @param diceRoll
 * @return true if the player is able to move
 */
    virtual int MakeDecision(int &movePieceIdx, int &diceRoll, enemyPiecePos &enemyPosition) = 0;

/**
 * This method is used to analyze what the players move did to the game
 * @param movePieceIdx
 * @param diceRoll
 * @return true if the player is able to move
 */
    virtual void GameAnalysis(int &movePieceIdx, int &diceRoll, enemyPiecePos &enemyPosition) = 0 ;


/**
 * Check which pieces are able to move given the dice roll and place them in the array
 * @param diceRoll
 * @param moveable
 * @return number of moveavble pieces
 */
int MovablePieces(int &diceRoll){
    // Counter to keep track of number of movable piece
    int nMoveable = 0;
    for (int i = 0; i < 4; i++){
        if (PiecePositions[i] == GOAL_POSITION){
            continue;
        }
        else if (PiecePositions[i] == HOME_POSITION) {
            // If it is a six or globus, pieces can be in the home position
            if (diceRoll == DIE_SIX || diceRoll == DIE_GLOBUS) {
                MovablePieceIdx[nMoveable]=i;
                nMoveable += 1;
            }
        }
            // If the pieces is past the last star. This include the whole goal area
        else if (PiecePositions[i] >= POS_LAST_STAR) {
            if ( !(diceRoll == DIE_STAR || diceRoll == DIE_GLOBUS )) {
                MovablePieceIdx[nMoveable]=i;
                nMoveable += 1;
            }
        }
            // If the peice is past the last globus, and before the last start
        else if (PiecePositions[i] >= POS_LAST_GLOBUS) {
            if ( diceRoll != DIE_GLOBUS) {
                MovablePieceIdx[nMoveable]=i;
                nMoveable += 1;
            }
        }
            // If it is not in the goal position, home position or past the last globus, then it must be moveable.
        else {
            MovablePieceIdx[nMoveable] = i;
            nMoveable += 1;
        }
    }
    return nMoveable;
}

/**
 * Roll the die
 * @return Random integer between 1 and 6
 */
    int RollDie() {
        return (rand() % 6)+1;
    }
/** Update the piece position
 * @param pieceIdx
 * @param posNew
 */
    void setPiecePosition(const int &pieceIdx, const int &posNew) {
        PiecePositions[pieceIdx] = posNew;
    }

/** Return the piece positions
 * @return
 */
    const std::array<int, 4> &getPiecePositions() const {
        return PiecePositions;
    }
/** Return player idx
 *
 * @return
 */
    int getPlayerIdx() const {
        return PlayerIdx;
    }
};



#endif //AI_PLAYERBASE_H
