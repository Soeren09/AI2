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



    int Reward;
    //Eigen::Matrix<int, 20, 1> NetworkInputState;
    Eigen::Matrix<int, 4, 1> NetworkInputState;
    // 90 RBF - lowest count for an activation function for 100 runs was 2
public:
    Network<4, 10, 4, 90> ActorCritic;

};


#endif //AI_PLAYERAC_H
