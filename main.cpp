//
// Created by soeren on 4/16/20.
//
#include <iostream>
#include <chrono>

#include "Game.h"
#include "Parameters.h"
using namespace std;


int main() {
    // There is a VERBOSE flag in Parameters.h

    int itt = 1000000;    // Breaks the txt file (only do one itr)
    int time[itt];
    array<int, 4> winners = {0,0,0,0};
    for (int i = 0; i< itt; i++)
    {
        auto start = chrono::high_resolution_clock::now();

        Game Board = Game();
        winners[Board.Run(0, "ludoReplay.txt")] += 1;
        Board.Reset();

        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        //cout << duration.count() << " microseconds" << endl;
        time[i] = duration.count();

    }
    int average = 0;
    for (int i = 0; i <itt; i++)
        average += time[i];
    cout << "Total time: "<< average << endl;
    average = average/itt;

    cout << "Time average over " << itt << " iterations: " << average << " microseconds" << endl << endl;

    cout << "Winner stats: " << endl;
    cout << "PLayer 0 won " << winners [0] << " times." <<endl;
    cout << "PLayer 1 won " << winners [1] << " times." <<endl;
    cout << "PLayer 2 won " << winners [2] << " times." <<endl;
    cout << "PLayer 3 won " << winners [3] << " times." <<endl;


    return 0;
}

