//
// Created by soeren on 5/1/20.
//
#include "PlayerAC.h"
#include <iostream>

#include <chrono>

using namespace std;

PlayerAC::PlayerAC(int playerIdx, const std::array<int, 4> &piecePositions) : PlayerBase::PlayerBase(playerIdx, piecePositions) {
    Reward = 0;
        // All pieces start at home
    CurState[0] = STATE_HOME;
    CurState[1] = STATE_HOME;
    CurState[2] = STATE_HOME;
    CurState[3] = STATE_HOME;

}


/**
 * This is the main function of this class. It rolls the dice and make the decision about what piece to move.
 * @return return true if able to make a move,
 */
int PlayerAC::MakeDecision(int &movePieceIdx, int &diceRoll, enemyPiecePos &enemyPosition) {
        // Check if a piece were killed by one of the other players
    UpdateState(enemyPosition);
        // TODO Right now the is no penalty when a piece dies

    // ########################### Start round ##############################################
        // Roll the dice
    diceRoll = RollDie();

        // Check to see which pieces are moveable
    nMovable = MovablePieces(diceRoll);


        // If the player is unable to move, skip turn
    if (nMovable == 0) {
        return 0;
    }

        // Convert the current state to a input state to the Actor/Critic Network
    //NetworkInputState.block(0,0,4,1) = Eigen::Matrix<int,4,1>(CurState.data());
    //NetworkInputState.block(4,0,4,1) = Eigen::Matrix<int,4,1>(PiecePositions.data());
    //NetworkInputState.block(8,0,12,1) = Eigen::Matrix<int,12,1>(enemyPosition.data());

    NetworkInputState = Eigen::Matrix<int,4,1>(CurState.data());

    movePieceIdx = ActorCritic.Actor(NetworkInputState, nMovable, MovablePieceIdx);

        // Store the state
    StoreState();
    return 1;


//    switch(diceRoll) {
//        case DIE_GLOBUS:
//            movePieceIdx = MovablePieceIdx[(rand() % nMovable)];
//            break;
//        case DIE_STAR:
//            movePieceIdx = MovablePieceIdx[(rand() % nMovable)];
//            break;
//        case DIE_SIX:
//            movePieceIdx = MovablePieceIdx[(rand() % nMovable)];
//            break;
//        default:    // normal move
//            movePieceIdx = MovablePieceIdx[(rand() % nMovable)];
//    }

    return 1;
}

void PlayerAC::GameAnalysis(int &movePieceIdx, int &diceRoll, enemyPiecePos &enemyPosition) {
    // Check how the internal state is.
    UpdateState(enemyPosition);
    // Get the reward
    GetReward();

    ActorCritic.Critic(Reward);
    ActorCritic.UpdateWeights();

    // Store the state
    StoreState();
}


/**
 * Recieve the reward for the previous move
 */
void PlayerAC::GetReward() {
        // Add the reward
    for ( int i = 0; i < 4; i ++){
        Reward += REWARD_TABLE[StoredState[i]][CurState[i]];
    }
}


/**
 * Save the state from last round, NewState, to the state of this round, CurState.
 * Then update the CurState, given the threats from the other players
 * NB: When the NewState is recieve by this function, it is already updated. That is, every killed piece internal
 * state has been update to STATE_HOME.
 * @param enemyPosition
 */
void PlayerAC::UpdateState(enemyPiecePos &enemyPosition) {
    for (int i = 0; i< 4 ; i++){
        if (PiecePositions[i] == HOME_POSITION ) {
            CurState[i] = STATE_HOME;
        }
        else if (PiecePositions[i] == GOAL_POSITION ) {
            CurState[i] = STATE_GOAL;
        }
        else if (PiecePositions[i] >= GOAL_AREA_START ) {
            CurState[i] = STATE_SAFE;
        } else {
            int FLAG_GLOBUS = 0;
                // Check if the piece is on a safe globus
            for (auto globusPos : POS_SAFE_GLOBUS){
                if (PiecePositions[i] == globusPos ) {
                    CurState[i] = STATE_SAFE;
                    FLAG_GLOBUS = 1;
                    break;
                }
            }
                // If it was not on a safe globus
            if (!FLAG_GLOBUS){
                    // Check if the piece is on a Enemy globus
                for (auto globusPos : POS_ENEMY_GLOBUS){
                    if (PiecePositions[i] == globusPos ) {
                        CurState[i] = STATE_HOSTILE;
                        FLAG_GLOBUS = 1;
                        break;
                    }
                }
                    // If it was not, then it must be on a NORMAL board position or STAR
                if (!FLAG_GLOBUS) {
                    CurState[i] = STATE_NEUTRAL;
                }
            }
        }
    }
        // If two pieces are located at the same position, then they are safe, except if it is another players home Globus
    for (int idx_i = 0; idx_i < 4; idx_i ++) {
            // If one is neutral at the current position, then the other must be aswell
        if (CurState[idx_i] == STATE_NEUTRAL) {
            for (int idx_j = idx_i + 1; idx_j < 4; idx_j++) {
                if (PiecePositions[idx_i] == PiecePositions[idx_j]) {
                    CurState[idx_i] = STATE_SAFE;
                    CurState[idx_j] = STATE_SAFE;
                }
            }
        }
    }

    // ################# Check all NEUTRAL pieces if they can be attacked #################

    // Check the threat from each enemy - Only valid if the piece is not at HOME, GOAL or SAFE position
    int nPossibleDangerPieces = 0;
    array<int,4> possibleDangerPieceIdx;
    for (int idx = 0; idx < 4; idx ++){
        if ( CurState[ idx ] == STATE_NEUTRAL ){
            possibleDangerPieceIdx[nPossibleDangerPieces] = idx;
            nPossibleDangerPieces += 1;
        }
    }
    // If there is no piece at a NEUTRAL location, then no point in check if it could be attacked
    if (nPossibleDangerPieces == 0) {
        return;
    }

    // Loop through every enemy position and check if it is threathening a pieces
    for (auto enemyPos : enemyPosition){
        // Check if the piece is not at a unreachable location - home of goal area
        if (enemyPos != UNREACHABLE_POSITION){
            // Check the enemy position against every player position
            for (int i = 0; i < nPossibleDangerPieces; i ++){
                // Skip the pieces that are already at a hostile position
                if (CurState[ possibleDangerPieceIdx[i] ] != STATE_HOSTILE){
                    // Check if the piece is at a hostile position
                    int currentPiecePosition = PiecePositions[ possibleDangerPieceIdx[i] ];
                    if (HostilePosition(currentPiecePosition, enemyPos) ) {
                        CurState[ possibleDangerPieceIdx[i] ] = STATE_HOSTILE;
                    }
                }
            }
        }
    }
}


/**
 * Store the current state so it can be used to calculate the reward
 */
void PlayerAC::StoreState() {
    // Save the states
    StoredState[0] = CurState[0];
    StoredState[1] = CurState[1];
    StoredState[2] = CurState[2];
    StoredState[3] = CurState[3];
}

/**
 * Check the player position against the enemy position and return whether is it a hostile position.
 * NB: The player piece position should not be in STATE_HOME, STATE_GOAL or STATE_SAFE
 * @param playerPiecePos piece position in local coordinate
 * @param enemyPiecePos piece position in player coordinate
 * @return
 */
int PlayerAC::HostilePosition(int &playerPiecePos, int &enemyPiecePos) {

    // ################### Check is the player is at a star position ##########################
    for (int i = 0; i < N_STAR; i++) {
        if (playerPiecePos == POS_STAR[i]) {
                // The player is in HOSTILE area if the player is between POS_STAR[i-2] and POS_START[i-1].
                // If it is one of the two first stars, then it need to handle from higher positions
            if ( i == 0) {
                if ( POS_STAR[N_STAR-2] <= enemyPiecePos && enemyPiecePos < POS_STAR[N_STAR-1]){
                    return 1;
                }
            }
            else if ( i == 1) {
                if ( enemyPiecePos == 0 || enemyPiecePos < POS_STAR[i-1]){
                    return 1;
                }
            }
            else {
                if ( POS_STAR[i-2] <= enemyPiecePos && enemyPiecePos < POS_STAR[i-1]){
                    return 1;
                }
            }
            return 0;
        }
    }

    // ################### Player is not on a star ##########################

        // Check if the piece is reachable from higher piece coordinates
    int pieceDifference = 100;
    if (playerPiecePos <= 6){
            // Make a roll over and check the position difference
        int newPlayerPos = playerPiecePos + 6;
        int newEnemyPos = (enemyPiecePos + 6) % GOAL_AREA_START ;
        // If the player piece is leading the enemy piece, and the difference is less than a dice roll
        if ( newPlayerPos > newEnemyPos ) {
            pieceDifference = newPlayerPos - newEnemyPos;
        }
    } else {
            // If the player piece is leading the enemy piece, and the difference is less than a dice roll
        if ( playerPiecePos > enemyPiecePos ) {
            pieceDifference = playerPiecePos - enemyPiecePos;
        }
    }

        // Check whether the enemy piece is closer than a dice roll from the player.
        // Enemy cannot roll the die value of a globus = 5 or a star = 3
    if (pieceDifference <= DIE_SIX) {
        if (pieceDifference != DIE_GLOBUS && pieceDifference != DIE_STAR ){
            return 1;
        }
    }
    return 0;
}
