//
// Created by soeren on 4/16/20.
//
// The Game class is a simple mechanic class that follows the rule of the game. That is, it recieve the dice roll and
// the desired piece that the current player wants to move, and go though a bunch of IF statements to check if the
// the peice is able to move given the dice roll.
// The class follows the rules specified in http://spilregler.dk/ludo/


#ifndef AI_GAME_H
#define AI_GAME_H

#include "Player.h"
#include <array>
#include <string>
#include <fstream>
#include <vector>


class Game {
public:
    struct collision {
        collision() {
            nCollisions = 0;
        }
            // Number of collisions
        int nCollisions;
            // Point set of <playerIdx, collisionPieceIdx>
        std::pair<std::vector<int>, std::vector<int>> collisions;
        void addCollision (int playerIdx, int pieceIdx){
            nCollisions +=1;
            collisions.first.push_back(playerIdx);
            collisions.second.push_back(pieceIdx);
        }
    };

    Game();
    int Run(int saveReplay, std::string replayPath);
    void Reset();
    int WinnerCheck(int PlayerIdx);
    int PlayerMove(int playerIdx, int PieceIdx, int diceRoll);
    int HandleStar(int playerIdx, int movePieceIdx);
    int HandleGlobus(int playerIdx, int movePieceIdx);
    int HandleSix(int playerIdx, int movePieceIdx);
    int HandleNormal(int playerIdx, int movePieceIdx, int dieRoll);
    void SendHome(int playerIdx, int pieceIdx);
    void MovePiece(int playerIdx, int pieceIdx, int pos);

    int BoardPositionCheck(int playerIdx, int PosBoard, collision &pieceCollision);
    void HandleCollision(int playerIdx, int movePieceIdx, int PosBoard, collision &pieceCollision, int type);
    int FindEnemyIdx(int playerIdx, int postIdx);

    void WriteStateToFile(int playerIdx, int dieRoll, int pieceIdx, std::ofstream &fout);

private:
    std::array<Player,4> Players;
};


#endif //AI_GAME_H
