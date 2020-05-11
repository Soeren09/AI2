//
// Created by soeren on 5/1/20.
//

#ifndef AI_PLAYERAC_H
#define AI_PLAYERAC_H


#include "PlayerBase.h"
#include "Parameters.h"
#include "Network.h"
#include "ActorCriticParameters.h"

class PlayerAC : public PlayerBase{
public:
    PlayerAC(int playerIdx, const std::array<int, 4> &piecePositions);


    int MakeDecision(int &movePieceIdx, int &diceRoll, enemyPiecePos &enemyPosition) override;
    void GameAnalysis(int &movePieceIdx, int &diceRoll, enemyPiecePos &enemyPosition) override;
    void GetReward();

    void UpdateState(enemyPiecePos &enemyPosition);
    void StoreState();
    void SetState(enemyPiecePos &enemyPosition);
    int HostilePosition(int &playerPiecePos, int &enemyPiecePos);
    int Actor (int &diceRoll, enemyPiecePos &enemyPosition);
    void NormalizeNetworkInputState();

private:
    // The player have three states.
    // CurState     - is the state that the pieces are in at the moment
    // StoredState  - is the state that the pieces are in when the player end his turn
    // PotState     - is the potential state each piece could be in given the dice roll
    actorCriticPieceState CurState;
    actorCriticPieceState StoredState;

    actorCriticPieceState PotState;

//    int nState, nHidden, nOutput;
//    Network<nState, nHiddenm nOutput> ActorCritic;


    Eigen::Matrix<int, 20, 1> NetworkInputState;
    //Network<20, 2*20+1, 4> ActorCritic;
    Network<20, 2, 4, 10> ActorCritic;

    int Reward;
};


#endif //AI_PLAYERAC_H
