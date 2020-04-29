//
// Created by soeren on 4/16/20.
//

#include "Player.h"
#include "Parameters.h"

#include <iostream>

using namespace std;

Player::Player() {}
/**
 * PLayer constructor.
 * @param playerIdx Global player idx number
 * @param enemyIdx Global index number of enemies
 * @param piecePosition Array of piece position: 0: Home, 1-51: Board zone, 52-56: Goal entrance, 57: Goal position
 */
Player::Player(int playerIdx, const array<int, 4> &piecePositions) : playerIdx(
        playerIdx), piecePositions(piecePositions) {
}

/**
 * Roll the die
 * @return Random integer between 1 and 6
 */
int Player::RollDie() {
    return (rand() % 6)+1;
}

/**
 * This is the main function of this class. It rolls the dice and make the decision about what piece to move.
 * @return return true if able to make a move,
 */
int Player::MakeMove(int &movePieceIdx, int &diceRoll){

    // Start by rolling the dice
    diceRoll = RollDie();

    int nMovable = MovablePieces(diceRoll);


    if (VERBOSE) {
        cout << "Moveable Pieces: " << nMovable << " Piece idx: ";
        for (int i = 0; i < nMovable; i++) {
            cout << movablePieceIdx[i] << " ";
        }
        cout << endl;
    }

        // If the player is unable to move
    if (nMovable == 0) {
        //cout << "Unable to move" << endl;
        return 0;
    }

    switch(diceRoll) {
        case DIE_GLOBUS:
                // Find the first piece place at home
            for (int i = 0; i < nMovable ; i++){
                if (piecePositions[movablePieceIdx[i]] == HOME_POSITION) {
                    movePieceIdx = i;
                    break;
                }
            }
                // No pieces at home
            if (movePieceIdx == -1){
                movePieceIdx = movablePieceIdx[(rand() % nMovable)];
            }
            // TODO generate random move
            movePieceIdx = movablePieceIdx[(rand() % nMovable)];
            break;
        case DIE_STAR:
            movePieceIdx = movablePieceIdx[(rand() % nMovable)];
            break;
        case DIE_SIX:
            movePieceIdx = movablePieceIdx[(rand() % nMovable)];
            break;
        default:    // normal move
            movePieceIdx = movablePieceIdx[(rand() % nMovable)];
    }

    return 1;

}


Player::~Player() {

}

/**
 * Check which pieces are able to move given the dice roll and place them in the array
 * @param diceRoll
 * @param moveable
 * @return number of moveavble pieces
 */
int Player::MovablePieces(int &diceRoll){

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

const std::array<int, 4> &Player::getPiecePositions() const {
    return piecePositions;
}

int Player::getPlayerIdx() const {
    return playerIdx;
}

void Player::setPiecePosition(const int &pieceIdx, const int &posNew) {
    piecePositions[pieceIdx] = posNew;
}
