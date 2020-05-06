//
// Created by soeren on 4/16/20.
//
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>

#include "Game.h"
#include "Parameters.h"

using namespace std;

Game::Game() {
    Players[0] = new PlayerRandom(0, std::array<int,4>{0,0,0,0}  );
    Players[1] = new PlayerRandom(1, std::array<int,4>{0,0,0,0}  );
    Players[2] = new PlayerRandom(2, std::array<int,4>{0,0,0,0}  );
    Players[3] = new PlayerRandom(3, std::array<int,4>{0,0,0,0}  );

        // Random shuffle the players
    shuffle (Players.begin(), Players.end(), std::mt19937(std::random_device()()));

    // Generate LUT:
    for (int i = 0; i < 52; i++) {
        LUT[i][0] = i;              // PLayer idx 0
        LUT[i][1] = (i + 39)%52;    // PLayer idx +1
        LUT[i][2] = (i + 26)%52;    // PLayer idx +2
        LUT[i][3] = (i + 13)%52;    // PLayer idx +3
    }
}

Game::Game(PlayerBase &p1, PlayerBase &p2, PlayerBase &p3, PlayerBase &p4){
    Players[0] = &p1;
    Players[1] = &p2;
    Players[2] = &p3;
    Players[3] = &p4;

    for (auto p: Players){
        if (!(p->GetReady())) {
            cout << "PlayerIdx: " << p->getPlayerIdx() << " is not ready. Reset player"<< endl;
            // TODO Reset is disabled
            //p->Reset();
        }
    }

    // TODO Shuffle disable
        // Random shuffle the players
    //shuffle (Players.begin(), Players.end(), std::mt19937(std::random_device()()));

    // Generate LUT:
    for (int i = 0; i < 52; i++) {
        LUT[i][0] = i;              // PLayer idx 0
        LUT[i][1] = (i + 39)%52;    // PLayer idx +1
        LUT[i][2] = (i + 26)%52;    // PLayer idx +2
        LUT[i][3] = (i + 13)%52;    // PLayer idx +3
    }
}

/**
 * This function is handling the full game
 * @param saveReplay Should the game be saved to a file
 * @param replayPath Where should the game be saved to
 * @return return the winner idx
 */
int Game::Run(int saveReplay=0, string replayPath="ludoReplay.txt"){

    ofstream myfile;
    // If replay is enables, open the file.
    if (saveReplay){
        remove(replayPath.c_str());
        myfile.open(replayPath);
    }

    int iteration_max = 2;
    for (int iteration = 0; iteration < iteration_max; iteration++)    {
        for (int playerTurn = 0; playerTurn < N_PLAYERS; playerTurn++) {
            int movePieceIdx = -1, diceRoll = -1;
            enemyPiecePos enemyPos;
            FindEnemyPos(playerTurn, enemyPos);

            if (Players[playerTurn]->MakeDecision(movePieceIdx, diceRoll, enemyPos)) {
                if ( !PlayerMove(playerTurn, movePieceIdx, diceRoll) ) {  // Not Valid move
                    // TODO Penalize the player
                    cout << "Player made choose an illegal move." << endl;
                }
                    // Analys the move made
                Players[playerTurn]->GameAnalysis(movePieceIdx, diceRoll, enemyPos);
            }
            else { // PLayer is unable to move
                // TODO handle when the player is unable to move
            }

            if (VERBOSE) {
                cout << "Print: " << "Player: " << playerTurn << " PlayerIdx: " << Players[playerTurn]->getPlayerIdx() << " DieRoll: " << diceRoll << " PieceIdx: " << movePieceIdx << endl << endl;
            }

            // Save the state
            if (saveReplay) {
                WriteStateToFile(playerTurn, diceRoll, movePieceIdx, myfile);
            }
                // Check for winner, and return if found
            if (WinnerCheck(playerTurn)) {
                if (VERBOSE) {
                    cout << playerTurn << " won in " << iteration << " iterations " << endl;
                }
                    // Set the ready flag to zero,
                for (auto p: Players){
                    p->GamePlayed();
                }
                return Players[playerTurn]->getPlayerIdx();
            }

//                // If player rolled a six, or has every piece at home, he can reroll
//            if (diceRoll == DIE_SIX || HomeCheck(playerTurn)) {
//                playerTurn -= 1;
//            }
        }
    }
        // Should get here!
    cout << "Shouldn't get here. Check Iterator count: iteration_max = " << iteration_max << endl;
    // Close the file
    if (saveReplay)
        myfile.close();
    return -1;
}

/**
 * Move the selected pice and handle the collission
 * @param playerIdx
 * @param PieceIdx
 * @param diceRoll
 * @return return 1 if the current player have a reroll: No pieces out or die = 6
 */
int Game::PlayerMove(const int &playerIdx, const int &PieceIdx, const int &diceRoll){
    switch(diceRoll) {
        case DIE_GLOBUS:
            return HandleGlobus(playerIdx, PieceIdx);
            break;
        case DIE_STAR:
            return HandleStar(playerIdx, PieceIdx);
            break;
        case DIE_SIX:
            return HandleSix(playerIdx, PieceIdx);
            break;
        default:    // normal move
            return HandleNormal(playerIdx, PieceIdx, diceRoll);
    }
    return 0;
}

/**
 * Function to handle a globus roll.
 * An invalied move is the player pick a pieces past or on the last globus.
 * A Enemy is only safe it he is on a safe globus, otherwise is he send home.
 * @param playerIdx
 * @param movePieceIdx
 * @return return true if the player were able to move
 */
int Game::HandleGlobus(const int &playerIdx, const int &movePieceIdx) {
    int posCur = Players[playerIdx]->getPiecePositions()[movePieceIdx];

    // The piece is past the last globus
    if (posCur >= POS_LAST_GLOBUS) {
        return 0;
    }

        // #####################  If the piece is at the home position  ##############################
    if (posCur == HOME_POSITION) {
        // Move piece to start position
        MovePiece(playerIdx, movePieceIdx, START_POSITION);
        // If the home position is occipied, then the piece will be send home
        collision collisions;  // first idx is the enemy idx, the second is the pieceIdx of the player
        // Send ENEMY home
        if (BoardCollisionCheck(playerIdx, START_POSITION, collisions) ) {
            for (int i = 0; i < collisions.nCollisions; i ++) {
                SendHome(collisions.collisions.first[i], collisions.collisions.second[i]);
            }
        }
        return 1;
        // #####################  If the piece is on the board  ##############################
    } else {
        // Check to see what the next globus position is
        for (auto posGlobus : POS_GLOBUS) {
            // Check if there is a next globus position
            if (posCur < posGlobus) {
                collision collisions;
                // Check is the is a piece already placed on the next globus located at posGlobus
                if (BoardCollisionCheck(playerIdx, posGlobus, collisions) ) {
                    HandleCollision(playerIdx, movePieceIdx, posGlobus, collisions, COLLISION_TYPE_GLOBUS);
                } else {
                    // Globus is free, just move the piece
                    MovePiece(playerIdx, movePieceIdx, posGlobus);
                }
                return 1;
            }
        }
    }
    // The piece cannot be moved
    return 0;
}

/**
 * Function to handle a star roll. An invalied moce is when a player pick a piecs past of on the last star and at the home position.
 * A enemy piece is only send home if he is at the end star, not a the intermediate start.
 * @param playerIdx
 * @param movePieceIdx
 * @return return true if the player were able to move
 */
int Game::HandleStar(const int playerIdx, const int movePieceIdx){
    int posCur = Players[playerIdx]->getPiecePositions()[movePieceIdx];

    // If the piece is past the last star (time 50 nano secounds)
    if (posCur >= POS_LAST_STAR || posCur == HOME_POSITION){
        return 0;
    }
        // Find the next star and jump
    for (int i = 0; i < N_STAR; i++) {
            // Check the next Star position
        if (posCur < POS_STAR[i]) {
            // Check if it is the last star -> Jump home
            if (POS_STAR[i] == POS_LAST_STAR) {
                MovePiece(playerIdx, movePieceIdx, GOAL_POSITION);
            }
                // Move to the next star and jump
            else {
                int posStar = POS_STAR[i + 1]; // End posistion

                    // Check if there is a piece placed on the next star located posStar
                collision collisions;
                if (BoardCollisionCheck(playerIdx, posStar, collisions) ) {
                    HandleCollision(playerIdx, movePieceIdx, posStar, collisions, COLLISION_TYPE_NORMAL);
                } else {
                    // Position is free, just move the piece
                    MovePiece(playerIdx, movePieceIdx, posStar);
                }
            }
            return 1;
        }
    }
    return 0;
}

/**
 * Function to handle a Six roll. When then the function is called. If the peice is at the home position it should move
 * out, otherwise it should act like a normal dice roll
 * @param playerIdx
 * @param movePieceIdx
 * @return return true if the player were able to move
 */
int Game::HandleSix(const int &playerIdx, const int &movePieceIdx){
    int posCur = Players[playerIdx]->getPiecePositions()[movePieceIdx];

    // #####################  If the piece is at the home position  ##############################
    if (posCur == HOME_POSITION) {
        // Move piece to start position
        MovePiece(playerIdx, movePieceIdx, START_POSITION);
        // If the home position is occipied, then the piece will be send home
        collision collisions;  // first idx is the enemy idx, the second is the pieceIdx of the player
        // Send ENEMY home
        if (BoardCollisionCheck(playerIdx, START_POSITION, collisions) ) {
            for (int i = 0; i < collisions.nCollisions; i ++) {
                SendHome(collisions.collisions.first[i], collisions.collisions.second[i]);
            }
        }
        return 1;
    }
       // If the piece is on the board move normal.
    return HandleNormal(playerIdx, movePieceIdx, DIE_SIX);
}

/**
 * Handle every normal dice roll. An invalid move is when the piece is at the home position.
 * When the player land on a star, he jumps and an enemy piece is only send home if he is at the end star,
 * not a the intermediate start.
 * @param playerIdx
 * @param movePieceIdx
 * @param dieRoll
 * @return
 */
int Game::HandleNormal(const int &playerIdx, const int &movePieceIdx, const int &dieRoll) {
    int posCur = Players[playerIdx]->getPiecePositions()[movePieceIdx];

        // Check if the piece is at the home position
    if ( posCur == HOME_POSITION ){
        return 0;
    }

        // If the piece is in the goal area, it can move backwards
    if (posCur >= GOAL_AREA_START ){
        int posNew = posCur + dieRoll;
            // Check if the new position is beyond - count backwards
        if (posNew > GOAL_POSITION) {
            posNew = GOAL_POSITION - posNew%GOAL_POSITION;
        }
            MovePiece(playerIdx, movePieceIdx, posNew);
    }
        // Else move normally - If a peice land on a start, it should jump to the next
    else {
        int posNew = posCur + dieRoll;
            // Check if the new position is a start
        for (int i = 0; i < N_STAR; i++ ){
                // If true, set posNew accordingly
            if (posNew == POS_STAR[i]){
                    // If its the last star, then move the piece to goal
                if (POS_STAR[i] == POS_LAST_STAR){
                    MovePiece(playerIdx, movePieceIdx, GOAL_POSITION);
                    return 0;
                } else {
                    posNew = POS_STAR[i+1];
                    break;
                }
            }
        }
            // Check if the new position is a globus
        int FLAG_GLOBUS = 0;
        for (auto posGlobus : POS_GLOBUS){
            if (posGlobus = posNew){
                posNew = posGlobus;
                FLAG_GLOBUS = 1;
                break;
            }
        }

        // Check if there is a piece placed on the new position
        collision collisions;
        if (BoardCollisionCheck(playerIdx, posNew, collisions) ) {
            if (FLAG_GLOBUS) {
                HandleCollision(playerIdx, movePieceIdx, posNew, collisions,COLLISION_TYPE_GLOBUS);
            } else {
                HandleCollision(playerIdx, movePieceIdx, posNew, collisions,COLLISION_TYPE_NORMAL);
            }
        } else {
            // Position is free, just move the piece
            MovePiece(playerIdx, movePieceIdx, posNew);
        }
    }
    return 1;
}


/**
 * Check if a given board position is occipied by another piece
 * @param playerIdx the posBoard is given w.r.t this player
 * @param PosBoard relative position to check
 * @param pieceCollision pair of enemyPLayerIdx and the collision piece
 * @return true if in collision, false otherwise
 */
int Game::BoardCollisionCheck(const int &playerIdx, const int &PosBoard, collision &pieceCollision){
    int collionsFlag = 0;

        // Find enemy player piece position. Start form i=1, because current player is i = 0
    for (int enemy = 1; enemy < N_PLAYERS; enemy++){
        int enemyPlayerIdx = FindEnemyIdx(playerIdx, enemy);
        for (int pieceIdx = 0; pieceIdx<4; pieceIdx++){     // Check each piece
                // A piece is not in collision if the piece is at its home position
            if (Players[enemyPlayerIdx]->getPiecePositions()[pieceIdx] == LUT[PosBoard][enemy] && LUT[PosBoard][enemy] != HOME_POSITION) {
                pieceCollision.addCollision(enemyPlayerIdx, pieceIdx);
                collionsFlag = 1;

                if (VERBOSE) {
                    cout << "COLLISION! PLayerIdx: " << playerIdx << " Pos: " << PosBoard << " EnemyIdx: " << enemyPlayerIdx << " PieceIdx: " << pieceIdx << " Pos: " << LUT[PosBoard][enemy] << endl;
                }
            }
        }
    }
    return collionsFlag;
}

/**
 * Function to hand collision, it will send home the relevant players
 * @param playerIdx is the moving player
 * @param movePieceIdx is the piece that the player moved
 * @param PosBoard is the position to where the piece is moved
 * @param pieceCollision is the other pieces positioned at the position
 * @param type COLLISION_TYPE_NORMAL / COLLISION_TYPE_GLOBUS
 * @return
 */
void Game::HandleCollision(const int &playerIdx, const int &movePieceIdx, const int &PosBoard, collision &pieceCollision, const int &type) {

        // Check if there is more than one piece at the board position . Send PLAYER home if true
    if (pieceCollision.nCollisions > 1 ) {
        SendHome(playerIdx, movePieceIdx);
    } else{
        // ######################### Normal board position #######################################
        if (type == COLLISION_TYPE_NORMAL){
                int enemyIdx = pieceCollision.collisions.first[0];
                int enemyPieceIdx = pieceCollision.collisions.second[0];

                SendHome(enemyIdx, enemyPieceIdx);
                MovePiece(playerIdx, movePieceIdx, PosBoard);
            }
            // ######################### Globus board position #######################################
        else if (type == COLLISION_TYPE_GLOBUS){
            // Only a single piece is on the globus.
            // Send PLAYER home if its a safe globus and send ENEMY home if its a private Globus (Only red is protected on red globus)
            int enemyIdx = pieceCollision.collisions.first[0];
            int enemyPieceIdx = pieceCollision.collisions.second[0];
            // Get enemy position in his own reference system
            int enemyPos = Players[enemyIdx]->getPiecePositions()[enemyPieceIdx];
            bool safe = false;

            for (auto pos : POS_SAFE_GLOBUS) {
                if (enemyPos == pos )  {
                    safe = true;
                    break;
                }
            }
            if (safe) { // Send player home
                SendHome(playerIdx, movePieceIdx);
            } else {    // Send Enemy home
                SendHome(enemyIdx, enemyPieceIdx);
                MovePiece(playerIdx, movePieceIdx, PosBoard);
            }
        }
        else {
            cout << "Unknown Collision type" << endl;
        }
    }
}


/**
 * Send home the piece
 * @param playerIdx
 * @param pieceIdx
 */
void Game::SendHome(const int &playerIdx, const int &pieceIdx){
    Players[playerIdx]->setPiecePosition(pieceIdx, HOME_POSITION);
}

/**
 * Move piece to pos
 * @param playerIdx
 * @param pieceIdx
 * @param pos
 */
void Game::MovePiece(const int &playerIdx, const int &pieceIdx, const int pos){
    Players[playerIdx]->setPiecePosition(pieceIdx, pos);
}

/**
 * Win condition is when a player have all its pieces in the goal zone
 * @return true if the player won
 */
int Game::WinnerCheck(const int &PlayerIdx) {
    for (const int& pos : Players[PlayerIdx]->getPiecePositions()) { // access by const reference
        if (pos != GOAL_POSITION)
            return 0;
    }
    return 1;
}

/**
 * Check if all the pieces is place at the home position
 * @return true if every piece is at home
 */
int Game::HomeCheck(const int &PlayerIdx) {
    for (const int& pos : Players[PlayerIdx]->getPiecePositions()) { // access by const reference
        if (pos != HOME_POSITION)
            return 0;
    }
    return 1;
}

/**
 * Given then player index and the clockwise following player, return player idx
 * @param playerIdx
 * @param postIdx
 * @return Enemy playerIdx
 */
int Game::FindEnemyIdx(const int &playerIdx, const int &postIdx) {
    return (playerIdx + postIdx)%4;
}

/**
 * Find the positions of the current players enemies, given in the players reference system
 * @param playerIdx The current player
 * @param enemyPosition The positions given relative the the current player
 */
void Game::FindEnemyPos(const int &playerIdx, enemyPiecePos &enemyPosition){
    int i = 0;
    //cout << playerIdx << endl;
    for(int enemy = 1; enemy < N_PLAYERS; enemy++){
        int enemyIdx = FindEnemyIdx(playerIdx, enemy);
        //cout << enemyIdx << " ";
        for (auto piecesPos : Players[enemyIdx]->getPiecePositions() ){
                // Check if they are in unreachable place
            if (piecesPos >= GOAL_AREA_START || piecesPos == HOME_POSITION){
                enemyPosition[i + (4 * (enemy-1))] = UNREACHABLE_POSITION;
            } else {
                enemyPosition[i + (4 * (enemy-1))] = LUT[piecesPos][N_PLAYERS-enemy];
            }
            //cout << piecesPos << " -> " << enemyPosition[i + (4 * (enemy-1))] << " | ";
            i +=1;
        }
        i = 0;
        //cout << endl;
    }
    //cout << endl;
}


void Game::WriteStateToFile(const int &playerIdx, const int &dieRoll, const int &pieceIdx, ofstream &fout){

    string output = "";
    output += to_string(playerIdx) + ' '; // PLayer turn
    output += to_string(dieRoll) + ' ' ;   // PLayer roll
    output += to_string(pieceIdx) + ' ';  // Moved piece

    for (auto player : Players){
        for(auto const& pos: player->getPiecePositions()) {
            output += to_string(pos) + ' ';
        }
    }
    output += '\n';
    fout << output;
    cout << output << endl;
}