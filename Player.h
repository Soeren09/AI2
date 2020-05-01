//
// Created by soeren on 4/16/20.
//

#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include <array>


class Player {
public:
    Player();
    Player(int playerIdx, const std::array<int, 4> &piecePositions);
    ~Player();

    int MakeMove(int &movePieceIdx, int &diceRoll);

    int RollDie();
    int MovablePieces(int &diceRoll);


        // Setter
    void setPiecePosition(const int &pieceIdx, const int &posNew);

        // Getter
    const std::array<int, 4> &getPiecePositions() const;
    int getPlayerIdx() const;

private:
    std::array<int, 4> piecePositions;
    int playerIdx;
    std::array<int,4> movablePieceIdx;

};


#endif //AI_PLAYER_H
