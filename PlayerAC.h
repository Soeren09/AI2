//
// Created by soeren on 5/1/20.
//

#ifndef AI_PLAYERAC_H
#define AI_PLAYERAC_H

#include "PlayerBase.h"
#include "Parameters.h"

class PlayerAC : public PlayerBase{
    using PlayerBase::PlayerBase;


/**
 * This is the main function of this class. It rolls the dice and make the decision about what piece to move.
 * @return return true if able to make a move,
 */
    int MakeDecision(int &movePieceIdx, int &diceRoll, std::array<int, 4*(N_PLAYERS-1)> &enemyPosition) override;



};


#endif //AI_PLAYERAC_H
