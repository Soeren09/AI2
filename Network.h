//
// Created by soeren on 5/6/20.
//

#ifndef AI_NETWORK_H
#define AI_NETWORK_H


#include <eigen3/Eigen/Dense>
#include <random>
#include <algorithm>
#include <math.h>       /* sqrt */
#include <fstream>

template < int nInput,  int nHidden, int nOutput, int nRBF> // you can even set to a default value here of C++'11
class Network {
private:
        // Random guassian generatios
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
    Eigen::Matrix<double, nOutput, 1> Noise;

        // Reward values

        // Exploration mechanism
    double ExplorationScaling;
    double V_max;
    double V_min;

        // Actor
    double Alpha; // Learning rate;
    int Decision;

    // Network parameters
    // Input
    Eigen::Matrix<double, nInput, 1> x;

    // Hidden weights and bias
    Eigen::Matrix<double, nInput, nHidden> W;
    Eigen::Matrix<double, nHidden, 1> b;
    // Hidden neuron synaptic input
    Eigen::Matrix<double, nHidden, 1> z; //  z = W^T*x + b
    // Hidden output, input to next layer
    Eigen::Matrix<double, nHidden, 1> h; //  h = g(z)

    // Output weight and bias
    Eigen::Matrix<double, nHidden, nOutput> V;
    Eigen::Matrix<double, nOutput, 1> c;
    // Output synaptic neuron input
    Eigen::Matrix<double, nOutput, 1> u; //  u = V^T*h + c
    // Output
    Eigen::Matrix<double, nOutput, 1> y; //  y = f(u)


        // Critic
    double Lambda;     // Learning rate
    double Vx_old;     // V(x(t-1))
    double Vx;         // V(x(t))
    double Gamma;      // Discount factor
    double Delta;      // reward + Gamma*Vx - Vx_old;

        // Network
    Eigen::Matrix<double, nRBF, 1> RBFout;
    Eigen::Matrix<double, nRBF, 1> y_j;
    Eigen::Matrix<double, nRBF, nInput> Mean;
    Eigen::Matrix<double, nRBF, 1> Beta;
    //Eigen::Matrix<double, nRBF, nInput> S_1; // covariance S⁻1
    // 1/(2*sigma^2), sigma = 1/points * sum(|x-mu|): https://mccormickml.com/2013/08/15/radial-basis-function-network-rbfn-tutorial/


public:

    Eigen::Matrix<double, nRBF, 1> RBFcount;

    Network();
    void read_record();

    int Actor(Eigen::Matrix<int, nInput,1> &x_, const int &nMovable, const std::array<int, 4> &MovablePieceIdx);
    void Critic(int &reward);
    void UpdateWeights();

    void NormalizeNetworkInputState();

        // Help functions Actor
    void SoftMax(Eigen::Matrix<double,nOutput, 1> &x, Eigen::Matrix<double, nOutput, 1> &y) ;
    void ReLu(Eigen::Matrix<double, nHidden, 1> &x, Eigen::Matrix<double, nHidden, 1> &y);
    Eigen::Matrix<double, nHidden, 1> ReLu_deriv (const Eigen::Matrix<double, nHidden, 1> &y);

        // Help functions Critic
    double GaussianKernel(const Eigen::Matrix<double, nInput, 1> &x_, const Eigen::Matrix<double, nInput, 1> &mean_, const double &beta_);

    void StoreWeights();

};


template<int nInput, int nHidden, int nOutput, int nRBF>
Network<nInput, nHidden, nOutput, nRBF>::Network() {
    // Initialize the random distribution
    distribution = std::normal_distribution<double> (0.0,1.0);
    generator.seed(time(0));

    // Initialize Actor network
    W = Eigen::Matrix<double, nInput, nHidden>::Random();
    b = Eigen::Matrix<double, nHidden, 1>::Random();


    V = Eigen::Matrix<double, nHidden, nOutput>::Random();
    c = Eigen::Matrix<double, nOutput, 1>::Random();

    // Initialize Critic network
    RBFcount = Eigen::Matrix<double, nRBF, 1>::Random();
    read_record();

    // Initialize learning parameters
        // TODO Fic these values
    Alpha = 0.001;  // 10^-3
    Lambda = 0.001; // 10^-3

    Gamma = 0.95;
    ExplorationScaling = 5.0;
    V_max = 69;
    V_min = -30;
}

/**
 * Calculate the output given the input. FeedForward Network
 * @param x_
 * @return
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
int Network<nInput, nHidden, nOutput, nRBF>::Actor(Eigen::Matrix<int, nInput,1> &x_, const int &nMovable, const std::array<int, 4> &MovablePieceIdx) {
//    Eigen::Matrix<double, nOutput, 1> Network<nInput, nHidden, nOutput>::Actor(Eigen::Matrix<int, nInput,1> &x_, const int &nMovable, const std::array<int, 4> &MovablePieceIdx) {
    x = x_.template cast<double> ();
    //NormalizeNetworkInputState();

//    // ** Save normalized states **
//    std::string output = "";
//    char filename[ ] = "States_4.csv";
//    std::fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);
//
//
//    //std::cout<< "------" << std::endl << x.transpose() << "  size: "<< x.size() << std::endl;
//    if (uidlFile.is_open())
//        for(int i = 0; i < x.size(); i++){
//            output += std::to_string(x[i]) + " ";
//     //       std::cout << i  << " ";
//        }
//    //std::cout << std::endl;
//        output += '\n';
//    //    std::cout << output << std::endl;
//        uidlFile << output;
//        uidlFile.close();
//    // ****************************


    // Add the error term
    for (int i =0; i < nOutput; i++){
        Noise(i, 0) = distribution(generator);
    }

    // Exploitation = exploration_scale * noise * min[1, max[0, (V_max-V(x(t)))/(V_max-V_min)]
    Noise = ExplorationScaling * Noise * std::min((double)1, std::max((double)0, (V_max-Vx)/(V_max-V_min)) );

    z =W.transpose()*x + b ;
    ReLu(z,h);

    u =V.transpose()*h + c + Noise;
    SoftMax(u,y);


    // ############################ Find the Output ################################################
    if ( std::isnan(y[0])){
        std::cout << "Nan detected return random" << std::endl;
        Decision = MovablePieceIdx[(rand() % nMovable)];
        return Decision;
    }

    // Get max probability
    double max = 0;
    std::array<int,4> max_idxs;
    int nMax_idxs = 0;

    // Find the piece with the highes probability, given that it is able to move.
    for (int i = 0; i < nMovable; i++){
        // Check if the current value is greater than the current max
        if (y[MovablePieceIdx[i] ] > max) {
            max = y[MovablePieceIdx[i] ];
            nMax_idxs = 0;
            max_idxs[nMax_idxs++] = MovablePieceIdx[i];
        }   // Check the current value is equal to the max value
        else if (y[MovablePieceIdx[i] ] == max){
            // Add the value to the max list
            max_idxs[nMax_idxs++] = MovablePieceIdx[i];
        }
    }

    Decision =  max_idxs[ rand()%nMax_idxs ];
    return Decision;
}

/**
 * Calculate the estimated error: Delta, based on a RBF network
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 * @param reward
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::Critic(int &reward){


    for (int i = 0; i < nRBF; i++){
        y_j(i) = GaussianKernel(x, Mean.row(i), Beta(i));
            //if (a_(i) > 0.001)
            //std::cout << "i= " << i << " " << a_(i) << " ";
    }
    //    int max_idx = 0;
    //    double max = 0;
    //    for (int i = 0; i < y_.size(); i++){
    //        if (y_(i) > max){
    //            max_idx = i;
    //            max = y_(i);
    //        }
    //    }
    //    RBFcount(max_idx) +=1;


        // y(x(t)) = a_j(x(t)) / sum(a_(x(t))
    y_j = y_j/ y_j.sum();

    Vx_old = Vx;
    Vx = RBFout.transpose() * y_j;
    // Delta(t) = r(t) + gamma* V(x(t)) - V(x(t-1))
    Delta = reward + Gamma*Vx - Vx_old;

    //std::cout << "Delta: " << Delta << std::endl;
}

/**
 * Calculate the Gausian kernel given the state x, the centroid and the beta value
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 * @param x_
 * @param mean_
 * @param beta_
 * @return
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
double
Network<nInput, nHidden, nOutput, nRBF>::GaussianKernel(
        const Eigen::Matrix<double, nInput, 1> &x_,
        const Eigen::Matrix<double, nInput, 1> &mean_,
        const double &beta_){


    double dist_sqare = 0; // = ||s_1 * (x - mu_j)||^2
    for (int i = 0; i < x_.size(); i++){
        dist_sqare+= pow(x_(i) - mean_(i), 2);
    }

    return exp(-beta_*dist_sqare);
}


/**
 * Do the gradient decent and update the weights
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::UpdateWeights(){

    // Update Actor
    // Update V and c (Softmax layer): Based on https://towardsdatascience.com/policy-based-reinforcement-learning-the-easy-way-8de9a3356083
    Eigen::Matrix<double, nOutput, 1> k = Eigen::Matrix<double, nOutput, 1>::Zero();
    k(Decision,0) = 1;
        // delta_0 = Delta * f'(u)
    Eigen::Matrix<double, nOutput, 1> delta_0 = Delta * (-(k- y) );
        // delta_1 = U * delta_0 * g'(z)
    Eigen::Matrix<double, nHidden, 1> delta_1 = (V*delta_0).cwiseProduct(ReLu_deriv(z));

    V = V - Alpha * h * delta_0.transpose();
    //std::cout << "V: \n" << h  << std::endl;
    c = c - Alpha * delta_0;

    W = W - Alpha * x * delta_1.transpose();
    c = c - Alpha * delta_0;


    // Update Critic
    RBFout = RBFout - Lambda * Delta *  y_j;
}

/**
* Normalize NetworkInputState so it consiste of a vector with values from [0,1].
* NetworkInputState[0:3]: CurState -  Range 0-4.
* NetworkInputState[4:7]: PiecePositions - Range 0 - 57.
* NetworkInputState[8:19]: EnemyPosiition - Range -1 - GOAL_AREA_START = .
*/
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::NormalizeNetworkInputState() {
    for (int i = 0; i < x.size(); i++){
        if (i < 4)
            x[i] = (x[i]) / 4; // (x - x_min) / (x_max - x_min)
        else if (i < 8)
            x[i] = (x[i]) / 57;
        else
            x[i] = (x[i] + 1) / (GOAL_AREA_START + 1);
    }
}


/**
 * Softmax layer:
 * Based on https://stackoverflow.com/questions/9906136/implementation-of-a-softmax-activation-function-for-neural-networks
 * @param x Input vector
 * @param y Output vector
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::SoftMax(Eigen::Matrix<double, nOutput, 1> &x, Eigen::Matrix<double, nOutput, 1> &y){
    y = x;
    double x_max = x.maxCoeff();
    for (int f = 0; f < y.rows(); f++) {
        y(f) = exp(y(f) - x_max);
    }
    y /= y.sum();
}


/**
 * ReLu layer
 * @param x Input vector
 * @param y Output vector
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::ReLu(Eigen::Matrix<double, nHidden, 1> &x, Eigen::Matrix<double, nHidden, 1> &y) {
    Eigen::Matrix<double, nHidden, 1> A(x.rows(), x.cols());
    for( int i = 0; i< x.rows(); ++i ) {
        for (int j = 0; j < x.cols(); ++j) {
            if (x(i, j) <= 0) {
                A(i, j) = (0.0);
            } else A(i, j) = (x(i, j));
        }
    }
}

/**
 * Calculate the derivative of the ReLu function
 * @param y
 * @return
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
Eigen::Matrix<double, nHidden, 1> Network<nInput, nHidden, nOutput, nRBF>::ReLu_deriv (const Eigen::Matrix<double, nHidden, 1> &y) {
    Eigen::Matrix<double, nHidden, 1> B(y.rows(), y.cols());
    for( int i = 0; i < y.rows(); ++i )
        for (int j=0; j < y.cols() ; ++j)
        {
            if (y(i,j) <= 0.0){
                B(i,j)=(0.0);
            }
            else B(i,j)=(1.0);
        }
    return std::move(B.matrix());
}

/** TODO Eigen::DenseCoeffsBase<Derived, 1>::Scalar& Eigen::DenseCoeffsBase<Derived, 1>::operator()(Eigen::Index, Eigen::Index), delete CSV and rerun
 * Read centroid and debiations from the CSV file
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::read_record(){
    // File pointer
    std::fstream fin;

    // Open an existing file
    //fin.open("RBFnormalized.csv", std::ios::in);
    fin.open("../RBF_4.csv", std::ios::in);

    // Read the Data from the file as String Vector
    std::vector<std::string> row;
    std::string line, word, temp;

        // Read an entire row and store it in a string variable 'line' - Frist row is a mean
    int idx = 0;
    while ( std::getline(fin, line)) {
        row.clear();
        //std::getline(fin, line);

        // used for breaking words
        std::stringstream s(line);

        // read every column data of a row and store it in a string variable, 'word'
        while (std::getline(s, word, ' ')) {
            // add all the column data of a row to a vector
            row.push_back(word);
        }

        // First values is Beta, the rest are for the centroid // convert string to double for comparision
        char* pEnd;
        Beta(idx) = std::strtod( row[0].c_str(), &pEnd );
        for (int i = 1; i < row.size(); i ++){
            Mean(idx,i-1) = std::strtod( row[i].c_str(), &pEnd );
        }

        if (idx > nRBF){
            std::cout << "Idx > nRFB" << std::endl;
        }
        idx += 1;
    }

    std::cout << "Mean: \n" << Mean << std::endl;
    std::cout << "Beta: \n" << Beta.transpose() << std::endl;
}

/**
 * Store the weights to a file
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 */
template<int nInput, int nHidden, int nOutput, int nRBF>
void Network<nInput, nHidden, nOutput, nRBF>::StoreWeights() {
//
//        // ** Save normalized states **
//    std::string output = "";
//    char filename[ ] = "Weights.csv";
//    std::fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);
//
//
//    W;
//    b;
//    V;
//    c;
//    RBFout;
//
//    //std::cout<< "------" << std::endl << x.transpose() << "  size: "<< x.size() << std::endl;
//    if (uidlFile.is_open()) {
//        for (int j = 0; j < W.size(); j++)
//            std::cout << W(0) << std::endl;
//            //std::cout << W.row() << std::endl;
//          //  for (int i = 0; i < (W.row()).size(); i++) {
//            //    output += std::to_string(W(j,i)) + " ";
//        }
//
//        output += '\n';

        //output += std::to_string()
}

    //std::cout << std::endl;
    //    std::cout << output << std::endl;
        //uidlFile << output;
        //uidlFile.close();
    // ****************************




#endif //AI_NETWORK_H
