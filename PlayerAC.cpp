//
// Created by soeren on 5/1/20.
//
#include "PlayerAC.h"



/**
 * This is the main function of this class. It rolls the dice and make the decision about what piece to move.
 * @return return true if able to make a move,
 */
int PlayerAC::MakeDecision(int &movePieceIdx, int &diceRoll, std::array<int, 4 * (N_PLAYERS - 1)> &enemyPosition) {

    // Start by rolling the dice
    diceRoll = RollDie();

    int nMovable = MovablePieces(diceRoll);

    // If the player is unable to move
    if (nMovable == 0) {
        return 0;
    }

    // TODO implement actor critic

    switch(diceRoll) {
        case DIE_GLOBUS:
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
};