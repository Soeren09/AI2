//
// Created by soeren on 4/29/20.
//

#ifndef AI_PLAYERRANDOM_H
#define AI_PLAYERRANDOM_H

#include "PlayerBase.h"
#include "Parameters.h"

class PlayerRandom : public PlayerBase {
private:

public:
    using PlayerBase::PlayerBase;


/**
 * This is the main function of this class. It rolls the dice and make the decision about what piece to move.
 * @return return true if able to make a move,
 */
    int MakeDecision(int &movePieceIdx, int &diceRoll, std::array<int, 4*(N_PLAYERS-1)> &enemyPosition) override {

        // Start by rolling the dice
        diceRoll = RollDie();

        int nMovable = MovablePieces(diceRoll);

        // If the player is unable to move
        if (nMovable == 0) {
            return 0;
        }

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


#endif //AI_PLAYERRANDOM_H
