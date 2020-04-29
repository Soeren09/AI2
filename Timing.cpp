#include <iostream>
#include <chrono>
#include <vector>
//#include <math.h>
#include <map>
#include <iterator>

using namespace std::chrono;
struct Triple {
    int pos[3];
};


int convert(int curPlayer, int curPos, int enemyPlayer)
{
    // There are 13 step different between each player. Player 0 map lead 13 from 1, 26 from 2 39 from 3.
    // Calculate the different between the two players
    int nOffset = enemyPlayer - curPlayer;
    if (nOffset < 0)
        nOffset = 4 + nOffset;
    // Calculate the enemy player position in en enemy player map
    int pos = curPos + 13*(4 - nOffset);
    if (pos > 51)
        return pos-52;
    return pos;
    //return (curPos + 13*(4 - nOffset))%52;
}

int convertLUT(int curPlayer, int curPos, int enemyPlayer, int LUT[52][4])
{
    // There are 13 step different between each player. Player 0 map lead 13 from 1, 26 from 2 39 from 3.
    // Calculate the different between the two players
    int nOffset = enemyPlayer - curPlayer;
    if (nOffset < 0)
        nOffset = 4 + nOffset;
    // Calculate the enemy player position in en enemy player map
    return LUT[curPos][nOffset];

}

std::vector<int> piecePositions(int &dice) {
    std::vector<int> A;
    A.push_back(0);
    A.push_back(1);
    A.push_back(2);
    A.push_back(3);
    return A;
}

int piecePositions(int &dice, std::vector<int> &A) {
    A.push_back(0);
    A.push_back(1);
    A.push_back(2);
    A.push_back(3);
    return 1;
}

int piecePositionsA(int &dice, std::array<int,4> &A) {
    A[0] = 0;
    A[1] = 1;
    A[2] = 2;
    A[3] = 3;
    return 4;
}



int main() {

//////////////////////////////////////////////////////////////////////////
//        // Test LUT
//    int LUT[52][4];
//    for (int i = 0; i < 52; i++)
//    {
//        LUT[i][0] = i;
//        LUT[i][1] = (i + 13)%52;
//        LUT[i][2] = (i + 26)%52;
//        LUT[i][3] = (i + 39)%52;
//    }
//
//    int test1 = convert(3, 49, 2);
//    int test2 = convertLUT(3, 49, 2, LUT);

//   std::cout << test1 << " " << test2 << std::endl;

//    int ri = rand()%52;
//    auto start = high_resolution_clock::now();
//    for (int i = 0; i< 1000000; i++){
//        convert(3,49, 1);
//        convert(1,49, 3);
//    }
//    auto stop = high_resolution_clock::now();
//    auto duration = duration_cast<microseconds>(stop - start);
//    std::cout << duration.count() << " microseconds" << std::endl;
//
/// ######################################################################################
//    start = high_resolution_clock::now();
//    for (int i = 0; i< 1000000; i++){
//        convertLUT(3,49, 1,LUT);
//        convertLUT(1,49, 3,LUT);
//    }
//    stop = high_resolution_clock::now();
//    duration = duration_cast<microseconds>(stop - start);
//    std::cout << duration.count() << " microseconds" << std::endl;


////////////////////////////////////////////////////////////////////////////
//            // Test mod
//    int posNew = 62;
//    auto start = high_resolution_clock::now();
//    for (int i = 0; i< 1000000; i++){
//        int b = 57 + 57 - posNew;
//        //std::cout << posNew << " " << b << std::endl;
//    }
//    auto stop = high_resolution_clock::now();
//    auto duration = duration_cast<microseconds>(stop - start);
//    std::cout << duration.count() << " microseconds" << std::endl;
//
//
//    //int posNew = 62;
//    start = high_resolution_clock::now();
//    for (int i = 0; i< 1000000; i++){
//        int b_ = 57 - posNew%57;
//        //std::cout << posNew << " " << b_ << std::endl;
//    }
//    stop = high_resolution_clock::now();
//    duration = duration_cast<microseconds>(stop - start);
//    std::cout << duration.count() << " microseconds" << std::endl;

//////////////////////////////////////////////////////////////////////////
    // Test vector return
    int dice = 6;
    auto start = high_resolution_clock::now();
    for (int i = 0; i< 1000000; i++){
        std::vector<int> A;
        piecePositions(dice, A);
        //for (auto p : A)
        //    std::cout << p << std::endl;

    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count() << " microseconds" << std::endl;


    start = high_resolution_clock::now();
    for (int i = 0; i< 1000000; i++){

        std::vector A_ = piecePositions(dice);
        //for (auto p : A_)
        //  std::cout << p << std::endl;
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count() << " microseconds" << std::endl;

    // Test vector return
    start = high_resolution_clock::now();
    for (int i = 0; i< 1000000; i++){

        std::array<int,4> A;
        int num = piecePositionsA(dice, A);
        //for (auto p : A)
        //    std::cout << p << std::endl;

    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count() << " microseconds" << std::endl;


    return 0;
}