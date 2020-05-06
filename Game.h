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
#include "PlayerBase.h"
#include "PlayerRandom.h"
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
    Game(PlayerBase &p1, PlayerBase &p2, PlayerBase &p3, PlayerBase &p4);

    int Run(int saveReplay, std::string replayPath);
    int WinnerCheck(const int &PlayerIdx);
    int HomeCheck(const int &PlayerIdx);
    int PlayerMove(const int &playerIdx, const int &PieceIdx, const int &diceRoll);

    int HandleStar(const int playerIdx, const int movePieceIdx);
    int HandleGlobus(const int &playerIdx, const int &movePieceIdx);
    int HandleSix(const int &playerIdx, const int &movePieceIdx);
    int HandleNormal(const int &playerIdx, const int &movePieceIdx, const int &dieRoll);

    void SendHome(const int &playerIdx, const int &pieceIdx);
    void MovePiece(const int &playerIdx, const int &pieceIdx, const int pos);

    int BoardCollisionCheck(const int &playerIdx, const int &PosBoard, collision &pieceCollision);
    void HandleCollision(const int &playerIdx, const int &movePieceIdx, const int &PosBoard, collision &pieceCollision, const int &type);
    int FindEnemyIdx(const int &playerIdx, const int &postIdx);
    void FindEnemyPos(const int &playerIdx, enemyPiecePos &enemyPosition);

    void WriteStateToFile(const int &playerIdx, const int &dieRoll, const int &pieceIdx, std::ofstream &fout);

private:
    std::array<PlayerBase*, 4> Players;
    std::array<std::array<int, 4>, 52> LUT;
};


#endif //AI_GAME_H
