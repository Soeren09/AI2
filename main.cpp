//
// Created by soeren on 4/16/20.
//
#include <iostream>
#include <chrono>

#include "Game.h"
#include "Parameters.h"

#include "PlayerRandom.h"
#include "PlayerAC.h"

#include <eigen3/Eigen/Dense>


#include "Network.h"

using namespace std;


int main() {
    // Generate players    // There is a VERBOSE flag in Parameters.h
    ////    for (auto row : REWARD_TABLE){
    ////        for (auto element : row) {
    ////            cout << element << " ";
    ////        }
    ////        cout << endl;
    ////    }
    ////    cout << endl;

    PlayerRandom p1(0, std::array<int,4>{0,0,0,0}  );
    PlayerRandom p2(1, std::array<int,4>{0,0,0,0}  );
    PlayerRandom p3(2, std::array<int,4>{0,0,0,0}  );
    PlayerRandom p4(3, std::array<int,4>{0,0,0,0}  );

    PlayerAC p5(4, std::array<int,4>{0,0,0,0}  );


    int itt = 1;    // Breaks the txt file (only do one itr)
    int time[itt];
    array<int, 5> winners = {0,0,0,0, 0};
    for (int i = 0; i< itt; i++)
    {
            // Make sure the players are at home position
        p1.Reset(); p2.Reset(); p3.Reset(); p4.Reset(); p5.Reset();

            // Start game
        auto start = chrono::high_resolution_clock::now();

        //Game Board = Game(p1, p2, p3, p4);
        Game Board = Game(p5, p2, p3, p4);
        winners[Board.Run(false, "ludoReplay.txt")] += 1;
        //winners[Board.Run(true, "ludoReplay.txt")] += 1;

        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        //cout << duration.count() << " microseconds" << endl;
        time[i] = duration.count();

            // Update players
        // TODO Update the network

            // Keep track of games
        if (!(i%10000)){
            cout << i << "/" << itt << endl;
            //cout << duration.count() << " microseconds" << endl;
        }
    }


    int average = 0;
    for (int i = 0; i <itt; i++)
        average += time[i];
    cout << "Total time/itt: "<< average/itt << endl;
    average = average/itt;

    cout << "Time average over " << itt << " iterations: " << average << " microseconds" << endl << endl;

    cout << "Winner stats: " << endl;
    cout << "PLayer 0 won " << winners [0] << " times." <<endl;
    cout << "PLayer 1 won " << winners [1] << " times." <<endl;
    cout << "PLayer 2 won " << winners [2] << " times." <<endl;
    cout << "PLayer 3 won " << winners [3] << " times." <<endl;
    cout << "PLayer 4 won " << winners [4] << " times." <<endl;

    return 0;
}

