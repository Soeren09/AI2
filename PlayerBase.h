//
// Created by soeren on 4/29/20.
//
#ifndef AI_PLAYERBASE_H
#define AI_PLAYERBASE_H

#include <array>
#include "Parameters.h"

class PlayerBase {
protected:
    int playerIdx;
    std::array<int, 4> piecePositions;
    std::array<int, 4> movablePieceIdx;

public:
    PlayerBase(){};
/**
 * PLayer constructor.
 * @param playerIdx Global player idx number
 * @param enemyIdx Global index number of enemies
 * @param piecePosition Array of piece position: 0: Home, 1-51: Board zone, 52-56: Goal entrance, 57: Goal position
*/
    PlayerBase(int playerIdx, const std::array<int, 4> &piecePositions) : playerIdx(
            playerIdx), piecePositions(piecePositions) {
    }
    ~PlayerBase(){}

/**
 * Reset the piece position to the homeposition
 */
    void Reset(){
        piecePositions[0] = HOME_POSITION;
        piecePositions[1] = HOME_POSITION;
        piecePositions[2] = HOME_POSITION;
        piecePositions[3] = HOME_POSITION;
    }


/** Function to make decision about what piece to move.
 * The method get two empty int from the game class which the function need to fill out.
 * @param movePieceIdx
 * @param diceRoll
 * @return true if the player is able to move
 */
    virtual int MakeDecision(int &movePieceIdx, int &diceRoll, std::array<int, 4*(N_PLAYERS-1)> &enemyPosition) = 0;


/**
 * Check which pieces are able to move given the dice roll and place them in the array
 * @param diceRoll
 * @param moveable
 * @return number of moveavble pieces
 */
    int MovablePieces(int &diceRoll){
        // Counter to keep track of number of moveable piece
        int nMoveable = 0;
        for (int i = 0; i < 4; i++){
            if (piecePositions[i] == GOAL_POSITION){
                continue;
            }
            else if (piecePositions[i] == HOME_POSITION) {
                // If it is a six or globus, pieces can be in the home position
                if (diceRoll == DIE_SIX || diceRoll == DIE_GLOBUS) {
                    movablePieceIdx[nMoveable]=i;
                    nMoveable += 1;
                }
            }
                // If the pieces is past the last star. This include the whole goal area
            else if (piecePositions[i] >= POS_LAST_STAR) {
                if (diceRoll != DIE_STAR || diceRoll != DIE_GLOBUS) {
                    movablePieceIdx[nMoveable]=i;
                    nMoveable += 1;
                }
            }
                // If the peice is past the last globus, and before the last start
            else if (piecePositions[i] >= POS_LAST_GLOBUS) {
                if ( diceRoll != DIE_GLOBUS) {
                    movablePieceIdx[nMoveable]=i;
                    nMoveable += 1;
                }
            }
                // If it is not in the goal position, home position or past the last globus, then it must be moveable.
            else {
                movablePieceIdx[nMoveable] = i;
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
        piecePositions[pieceIdx] = posNew;
    }

/** Return the piece positions
 * @return
 */
    const std::array<int, 4> &getPiecePositions() const {
        return piecePositions;
    }
/** Return player idx
 *
 * @return
 */
    int getPlayerIdx() const {
        return playerIdx;
    }
};



#endif //AI_PLAYERBASE_H
