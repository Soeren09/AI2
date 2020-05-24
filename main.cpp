//
// Created by soeren on 4/16/20.
//
#include <iostream>
#include <chrono>
#include <vector>

#include "Game.h"
#include "Parameters.h"

#include "PlayerRandom.h"
#include "PlayerAC.h"

#include <eigen3/Eigen/Dense>


#include "Network.h"

using namespace std;



void SaveWinrate(double winrate, int newline){
    std::string output = "";
    char filename[ ] = "Winrate.csv";
    std::fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);

    if (uidlFile.is_open()) {
        if(newline){
            output += "\n";
        }
        else {
            output += to_string(winrate) + " ";
        }
        uidlFile << output;
        uidlFile.close();
    }
}

void SaveParam(double param){
    std::string output = "";
    char filename[ ] = "Param.csv";
    std::fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);

    if (uidlFile.is_open()) {
        output += to_string(param) + " ";
        output += "\n";
        uidlFile << output;
        uidlFile.close();
    }
}



int main() {
    // Generate players    // There is a VERBOSE flag in Parameters.h

    PlayerRandom p1(0, std::array<int,4>{0,0,0,0}  );
    PlayerRandom p2(1, std::array<int,4>{0,0,0,0}  );
    PlayerRandom p3(2, std::array<int,4>{0,0,0,0}  );
    PlayerRandom p4(3, std::array<int,4>{0,0,0,0}  );

    PlayerAC p5(4, std::array<int,4>{0,0,0,0}  );

    vector<int> reward;
   // vector<int> Itr;

    double alpha_[] = {0.0001, 0.00005, 0.00001};
    double lambda_[] = {0.0005, 0.0001, 0.00005};
    double gamma_[] = {0.8, 0.85, 0.90, 0.95, 1};
    double beta_[] = {0.4, 0.6, 0.8, 1.0, 1.2, 1.4};
    //for (auto alpha_p : alpha_) {
    //for (auto lambda_p : lambda_) {
    //for (auto gamma_p : gamma_) {
    //for (auto beta_p : beta_) {

        double alpha = alpha_[1];   // Fixed
        double lambda= lambda_[2];  // Fixed
        double gamma = gamma_[4];
        double beta = beta_[3];

        cout << "alpha: " << alpha  << " " <<
                "lambda: "<< lambda << " " <<
                "gamma: " << gamma << " " <<
                "bata " << beta <<  endl;

        SaveParam(4);
        p5.ActorCritic.ResetNetwork();
        p5.ActorCritic.SetLearningParamenters(alpha, lambda, gamma, beta);

        int itt = 300;    // Breaks the txt file (only do one itr)
        int time[itt];
        array<int, 5> winners = {0, 0, 0, 0, 0};
        for (int i = 0; i < itt; i++) {
            // Make sure the players are at home position
            p1.Reset();
            p2.Reset();
            p3.Reset();
            p4.Reset();
            p5.Reset();

            // ***** Start game ******
            auto start = chrono::high_resolution_clock::now();

            //Game Board = Game(p1, p2, p3, p4);
            Game Board = Game(p5, p2, p3, p4);
            //Itr.push_back(Board.Run(false, "ludoReplay.txt"));
            winners[Board.Run(false, "ludoReplay.txt")] += 1;

            auto stop = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
            time[i] = duration.count();
            // ******************************************************************************

            // Check the current weights
            if (!(i % 1)) {
                p5.SetUpdateFlag(0);    // Disable network update

                int games = 500;
                double wins = 0;
                for (int game = 0; game < games; game++) {
                    // Make sure the players are at home position
                    p1.Reset();
                    p2.Reset();
                    p3.Reset();
                    p4.Reset();
                    p5.Reset();

                    if (Board.Run(false, "ludoReplay.txt") == 4) {
                        wins += 1;
                    }
                }
                double winrate = wins / games;
                SaveWinrate(winrate, 0);
                // Store weights
                p5.ActorCritic.StoreWeights("Weights.csv");

                cout << "Winrate a iteration " << i << " is " << winrate << endl;
                p5.SetUpdateFlag(1);    // Enable network update
            }

            //reward.push_back(p5.Reward);

            //            // Keep track of games
            //        if (!(i%10000)){
            //            cout << i << "/" << itt << endl;
            //            //cout << duration.count() << " microseconds" << endl;
            //        }
        }
        SaveWinrate(2, 1);


        //const auto [min, max] = std::minmax_element(begin(reward), end(reward));
        //std::cout << "min = " << *min << ", max = " << *max << '\n';

//    const auto [min, max] = std::minmax_element(begin(Itr), end(Itr));
//    int avg = 0;
//    for (auto i : Itr)
//        avg += i;
//    avg/itt;
//    std::cout << "min = " << *min << ", max = " << *max << ", average = "<<  avg/itt << '\n';

        int average = 0;
        for (int i = 0; i < itt; i++)
            average += time[i];
        cout << "Total time/itt: " << average / itt << endl;
        average = average / itt;

        cout << "Time average over " << itt << " iterations: " << average << " microseconds" << endl << endl;

        cout << "Winner stats: " << endl;
        cout << "PLayer 0 won " << winners[0] << " times." << endl;
        cout << "PLayer 1 won " << winners[1] << " times." << endl;
        cout << "PLayer 2 won " << winners[2] << " times." << endl;
        cout << "PLayer 3 won " << winners[3] << " times." << endl;
        cout << "PLayer 4 won " << winners[4] << " times." << endl;
//    cout << "Winrate "
//            << (double)winners[0]/itt << "\t"
//            << (double)winners[1]/itt << "\t"
//            << (double)winners[2]/itt << "\t"
//            << (double)winners[3]/itt << endl;

        cout << "Actor/Critic win rate:" << (double) winners[4] / itt << endl;
        cout << (double) winners[4] / itt << endl;

    //}
    return 0;
}

