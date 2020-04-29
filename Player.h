//
// Created by soeren on 4/16/20.
//

#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include <array>
#include <vector>


class Player {
public:
    Player();
    Player(int playerIdx, const std::array<int, 4> &piecePositions);

    int RollDie();
    int MakeMove(int &movePieceIdx, int &diceRoll);
    int MovablePieces(int &diceRoll, std::array<int,4> &movablePieceIdx);



        // Setter
    //void setPiecePositions(const std::array<int, 4> &piecePositions);
    void setPiecePosition(const int pieceIdx, const int posNew);

        // Getter
    const std::array<int, 4> &getPiecePositions() const;
    const std::array<int, 3> &getEnemyIdx() const;
    const std::array<std::array<int, 4>, 52> &getLut() const;
    int getPlayerIdx() const;
    ~Player();

private:
    std::array<int, 4> piecePositions;
    int playerIdx;
    std::array<std::array<int, 4>, 52> LUT;

};


#endif //AI_PLAYER_H
