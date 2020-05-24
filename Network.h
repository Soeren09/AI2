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

template < int nInput,  int nHidden, int nOutput> // you can even set to a default value here of C++'11
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
    Eigen::Matrix<double, nInput, 1> x_normalized;

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
    Eigen::Matrix<double, 5,  1> RBFa_mean;   // 4 pieces x 5 states
    //Eigen::Matrix<double, 58, 1> RBFb_mean;  // 4 pieces x 58 different positions: pos 0-57
    //Eigen::Matrix<double, 53, 1> RBFc_mean; // 12 enemy pieces x 53 different positions: pos -1,0,...,51
    double Beta;
//
//    Eigen::Matrix<double, 116, 1> RBFout;
//    Eigen::Matrix<double, 116, 1> y_j;
    Eigen::Matrix<double, 5, 1> RBFout;
    Eigen::Matrix<double, 5, 1> y_j;
    //Eigen::Matrix<double, nRBF, nInput> Mean;
    //Eigen::Matrix<double, nRBF, 1> Beta;
    //Eigen::Matrix<double, nRBF, nInput> S_1; // covariance S⁻1
    // 1/(2*sigma^2), sigma = 1/points * sum(|x-mu|): https://mccormickml.com/2013/08/15/radial-basis-function-network-rbfn-tutorial/


public:

    //Eigen::Matrix<double, nRBF, 1> RBFcount;

    Network();
    void read_record();
    void SetRBF();

    int Actor(Eigen::Matrix<int, nInput,1> &x_, const int &nMovable, const std::array<int, 4> &MovablePieceIdx);
    void Critic(double &reward);
    void UpdateWeights();

    void NormalizeNetworkInputState();

        // Help functions Actor
    void SoftMax(Eigen::Matrix<double,nOutput, 1> &x, Eigen::Matrix<double, nOutput, 1> &y) ;
    void ReLu(Eigen::Matrix<double, nHidden, 1> &x, Eigen::Matrix<double, nHidden, 1> &y);
    Eigen::Matrix<double, nHidden, 1> ReLu_deriv (const Eigen::Matrix<double, nHidden, 1> &y);

        // Help functions Critic
    //double GaussianKernel(const Eigen::Matrix<double, nInput, 1> &x_, const Eigen::Matrix<double, nInput, 1> &mean_, const double &beta_);
    double GaussianKernel(const double &x_, const double &mean_);

    void StoreWeights(std::string path);
    void ResetNetwork();
    void SetLearningParamenters(double alpha, double lambda, double gamma, double beta );
};


template<int nInput, int nHidden, int nOutput>
Network<nInput, nHidden, nOutput>::Network() {
    // Initialize the random distribution
    distribution = std::normal_distribution<double> (0.0,1.0);
    generator.seed(time(0));

    // Initialize Actor network
    W = Eigen::Matrix<double, nInput, nHidden>::Random();
    b = Eigen::Matrix<double, nHidden, 1>::Zero();

    V = Eigen::Matrix<double, nHidden, nOutput>::Zero();
    c = Eigen::Matrix<double, nOutput, 1>::Zero();

    // Initialize Critic network
//    RBFout = Eigen::Matrix<double, 116, 1>::Random();
    RBFout = Eigen::Matrix<double, 5, 1>::Random();
    //RBFcount = Eigen::Matrix<double, nRBF, 1>::Random();
    //read_record();
    SetRBF();
    Beta = 1;                                       // Tune

    // Initialize learning parameters
        // TODO Fic these values
    Alpha = 0.001;  // 10^-3                        // Tune
    Lambda = 0.0001; // 10^-3                       // Tune

    Gamma = 0.95;                                   // Tune
    ExplorationScaling = 2.0;                       // Tune
    V_max = 243;        // Found by 100000 itr
    V_min = -87;
}

/**
 * Reset the network
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 */
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::ResetNetwork() {
    // Initialize the random distribution
    distribution = std::normal_distribution<double> (0.0,1.0);
    generator.seed(time(0));

    // Initialize Actor network
    W = Eigen::Matrix<double, nInput, nHidden>::Random();
    b = Eigen::Matrix<double, nHidden, 1>::Zero();

    V = Eigen::Matrix<double, nHidden, nOutput>::Zero();
    c = Eigen::Matrix<double, nOutput, 1>::Zero();

    // Initialize Critic network
//    RBFout = Eigen::Matrix<double, 116, 1>::Random();
    RBFout = Eigen::Matrix<double, 5, 1>::Random();
    //RBFcount = Eigen::Matrix<double, nRBF, 1>::Random();
    //read_record();
    SetRBF();
    Beta = 1;                                       // Tune

    // Initialize learning parameters
    // TODO Fic these values
    Alpha = 0.001;  // 10^-3                        // Tune
    Lambda = 0.0001; // 10^-3                       // Tune

    Gamma = 0.95;                                   // Tune
    ExplorationScaling = 1.0;                       // Tune
    V_max = 243;        // Found by 100000 itr
    V_min = -87;
}


/**
 * Calculate the output given the input. FeedForward Network
 * @param x_
 * @return
 */
template<int nInput, int nHidden, int nOutput>
int Network<nInput, nHidden, nOutput>::Actor(Eigen::Matrix<int, nInput,1> &x_, const int &nMovable, const std::array<int, 4> &MovablePieceIdx) {
//    Eigen::Matrix<double, nOutput, 1> Network<nInput, nHidden, nOutput>::Actor(Eigen::Matrix<int, nInput,1> &x_, const int &nMovable, const std::array<int, 4> &MovablePieceIdx) {
    x = x_.template cast<double> ();
    //StoreWeights();

        // Add the error term
    for (int i =0; i < nOutput; i++){
        Noise(i, 0) = distribution(generator);
    }

    // Exploitation = exploration_scale * noise * min[1, max[0, (V_max-V(x(t)))/(V_max-V_min)]
    Noise = ExplorationScaling * Noise * std::min((double)1, std::max((double)0, (V_max-Vx)/(V_max-V_min)) );
    //std::cout << "Noise: " << std::min((double)1, std::max((double)0, (V_max-Vx)/(V_max-V_min)) )<< std::endl;

    //NormalizeNetworkInputState();
    //z = W.transpose()*x_normalized + b ;

    z = W.transpose()*x + b ;
    ReLu(z,h);

    u = V.transpose()*h + c;// + Noise;
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

    // Find the piece with the highest probability, given that it is able to move.
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
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::Critic(double &reward){

//    y_j = Eigen::Matrix<double, 116, 1>::Zero();
    y_j = Eigen::Matrix<double, 5, 1>::Zero();
        // Calculate RBF for x[0:3]: state
    for (int i = 0; i < 4; i ++) {
        for (int j = 0; j < RBFa_mean.size(); j++) {
            y_j(j,0) += GaussianKernel(x(i,0), RBFa_mean(j,0));
        }
    }

//        // Calculate RBF for x[4:7]: state
//    for (int i = 4; i < 8; i ++) {
//        for (int j = 0; j <  RBFb_mean.size(); j++) {
//            y_j(RBFa_mean.size() + j,0) += GaussianKernel(x(i,0), RBFb_mean(j,0));
//        }
//    }
//
//        // Calculate RBF for x[8:19]: state
//    for (int i = 8; i < 20; i ++) {
//        for (int j = 0; j <  RBFc_mean.size(); j++) {
//            y_j(RBFa_mean.size() + RBFb_mean.size() + j,0) += GaussianKernel(x(i,0), RBFc_mean(j,0));
//        }
//    }
//    std::cout << "\n" << y_j.transpose() << "\n";


        // y(x(t)) = a_j(x(t)) / sum(a_(x(t))
    y_j = y_j/ y_j.sum();

    Vx_old = Vx;
    Vx = RBFout.transpose() * y_j;
    // Delta(t) = r(t) + gamma* V(x(t)) - V(x(t-1))
    Delta = reward + Gamma*Vx - Vx_old;
    //Delta = reward - Gamma*Vx + Vx_old;

//    std::cout << "Vx: " << Vx << std::endl;
//    std::cout << "Delta: " << Delta << std::endl;
}

/**
 * Calculate the Gausian kernel given the state x, the centroid and the beta value
 * @param x_
 * @param mean_
 * @param beta_
 * @return
 */
template<int nInput, int nHidden, int nOutput>
double Network<nInput, nHidden, nOutput>::GaussianKernel(const double &x_, const double &mean_){
    return exp(-Beta*pow(x_ - mean_, 2));
}

/**
 * Do the gradient decent and update the weights
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 */
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::UpdateWeights(){

    // Update Actor
    // Update V and c (Softmax layer): Based on https://towardsdatascience.com/policy-based-reinforcement-learning-the-easy-way-8de9a3356083
    Eigen::Matrix<double, nOutput, 1> k = Eigen::Matrix<double, nOutput, 1>::Zero();
    k(Decision,0) = 1;
        // delta_0 = Delta * f'(u)
    Eigen::Matrix<double, nOutput, 1> delta_0 = Delta * (-(k- y) );
        // delta_1 = U * delta_0 * g'(z)
    Eigen::Matrix<double, nHidden, 1> delta_1 = (V*delta_0).cwiseProduct(ReLu_deriv(z));

    V = V - Alpha * h * delta_0.transpose() ;
    c = c - Alpha * delta_0;

    W = W - Alpha * x * delta_1.transpose();
    b = b - Alpha * delta_1;

    // Update Critic
    RBFout = RBFout - Lambda * Delta *  y_j;
}

/**
* Normalize NetworkInputState so it consiste of a vector with values from [0,1].
* NetworkInputState[0:3]: CurState -  Range 0-4.
* NetworkInputState[4:7]: PiecePositions - Range 0 - 57.
* NetworkInputState[8:19]: EnemyPosiition - Range -1 - GOAL_AREA_START = .
*/
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::NormalizeNetworkInputState() {
    for (int i = 0; i < x.size(); i++){
        if (i < 4)
            x_normalized[i] = (x[i]) / 4; // (x - x_min) / (x_max - x_min)
        else if (i < 8)
            x_normalized[i] = (x[i]) / 57;
        else
            x_normalized[i] = (x[i] + 1) / (GOAL_AREA_START + 1);
    }
}


/**
 * Softmax layer:
 * Based on https://stackoverflow.com/questions/9906136/implementation-of-a-softmax-activation-function-for-neural-networks
 * @param x Input vector
 * @param y Output vector
 */
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::SoftMax(Eigen::Matrix<double, nOutput, 1> &x, Eigen::Matrix<double, nOutput, 1> &y){
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
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::ReLu(Eigen::Matrix<double, nHidden, 1> &x, Eigen::Matrix<double, nHidden, 1> &y) {
    Eigen::Matrix<double, nHidden, 1> A(x.rows(), x.cols());
    for( int i = 0; i< x.rows(); ++i ) {
        for (int j = 0; j < x.cols(); ++j) {
            if (x(i, j) <= 0) {
                A(i, j) = (0.0);
            } else A(i, j) = (x(i, j));
        }
    }
    y = A;
}

/**
 * Calculate the derivative of the ReLu function
 * @param y
 * @return
 */
template<int nInput, int nHidden, int nOutput>
Eigen::Matrix<double, nHidden, 1> Network<nInput, nHidden, nOutput>::ReLu_deriv (const Eigen::Matrix<double, nHidden, 1> &y) {
    Eigen::Matrix<double, nHidden, 1> B(y.rows(), y.cols());
    for( int i = 0; i < y.rows(); ++i )
        for (int j=0; j < y.cols() ; ++j)        {
            if (y(i,j) <= 0.0){
                B(i,j)=(0.0);
            }
            else B(i,j)=(1.0);
        }
    return std::move(B.matrix());
}



/**
 * Set the RBF network
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF */
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::SetRBF(){
    for (double i = 0; i <= 4; i ++){
        RBFa_mean(i,0) = i;
    }
//    for (double i = 0; i <= 57; i ++){
//        RBFb_mean(i,0) = i;
//    }
//    for (double i = -1; i <= 51; i ++){
//        RBFc_mean(i+1,0) = i;
//    }
//    std::cout << "a\n" << RBFa_mean << std::endl;
//    std::cout << "b\n" << RBFb_mean << std::endl;
//    std::cout << "v\n" << RBFc_mean << std::endl;
}


/**
 * Set learning parameters
 * @param alpha
 * @param lambda
 * @param gamma
 * @param beta
 */
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::SetLearningParamenters(double alpha, double lambda, double gamma, double beta ){
    Alpha = alpha;
    Lambda = lambda;
    Gamma = gamma;
    Beta = beta;
}


/**
 * Store the weights to a file
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 */
template<int nInput, int nHidden, int nOutput>
void Network<nInput, nHidden, nOutput>::StoreWeights(std::string filename_) {

//    std::cout << "W: " << W.rows() << " " << W.cols() << " " << W.size() << std::endl;
//    std::cout << "b: " << b.rows() << " " << b.cols() << " " << b.size() << std::endl;
//    std::cout << "V: " << V.rows() << " " << V.cols() << " " << V.size() << std::endl;
//    std::cout << "c: " << c.rows() << " " << c.cols() << " " << c.size() << std::endl;

    std::string output = "";
    char filename[ ] = "Weights.csv";
    std::fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);

    if (uidlFile.is_open()) {
        // W
        for (int j = 0; j < W.size(); j++) { // Size is the size of the whole matrix rows*cols
            output += std::to_string(W(j)) + " ";
        }
        output += '\n';
        // b
        for (int j = 0; j < b.size(); j++) { // Size is the size of the whole matrix rows*cols
            output += std::to_string(b(j)) + " ";
        }
        output += '\n';
        // V
        for (int j = 0; j < V.size(); j++) { // Size is the size of the whole matrix rows*cols
            output += std::to_string(V(j)) + " ";
        }
        output += '\n';
        // c
        for (int j = 0; j < c.size(); j++) { // Size is the size of the whole matrix rows*cols
            output += std::to_string(c(j)) + " ";
        }
        output += '\n';
        // RBF
        for (int j = 0; j < RBFout.size(); j++) { // Size is the size of the whole matrix rows*cols
            output += std::to_string(RBFout(j)) + " ";
        }
        output += '\n';
        output += std::to_string(Delta) + " ";
        output += '\n';
        uidlFile << output;
        uidlFile.close();
    }
}




#endif //AI_NETWORK_H


//template<int nInput, int nHidden, int nOutput>
//void Network<nInput, nHidden, nOutput>::Critic(int &reward){

//
//    for (int i = 0; i < nRBF; i++){
//        y_j(i) = GaussianKernel(x, Mean.row(i), Beta(i));
//            //if (a_(i) > 0.001)
//            //std::cout << "i= " << i << " " << a_(i) << " ";
//    }
//    //    int max_idx = 0;
//    //    double max = 0;
//    //    for (int i = 0; i < y_.size(); i++){
//    //        if (y_(i) > max){
//    //            max_idx = i;
//    //            max = y_(i);
//    //        }
//    //    }
//    //    RBFcount(max_idx) +=1;
//
//
//        // y(x(t)) = a_j(x(t)) / sum(a_(x(t))
//    y_j = y_j/ y_j.sum();
//
//    Vx_old = Vx;
//    Vx = RBFout.transpose() * y_j;
//    // Delta(t) = r(t) + gamma* V(x(t)) - V(x(t-1))
//    Delta = reward + Gamma*Vx - Vx_old;
//
//    //std::cout << "Delta: " << Delta << std::endl;
//}


/** TODO Eigen::DenseCoeffsBase<Derived, 1>::Scalar& Eigen::DenseCoeffsBase<Derived, 1>::operator()(Eigen::Index, Eigen::Index), delete CSV and rerun
 * Read centroid and debiations from the CSV file
 * @tparam nInput
 * @tparam nHidden
 * @tparam nOutput
 * @tparam nRBF
 */
//template<int nInput, int nHidden, int nOutput>
//void Network<nInput, nHidden, nOutput>::read_record(){
//    // File pointer
//    std::fstream fin;
//
//    // Open an existing file
//    //fin.open("RBFnormalized.csv", std::ios::in);
//    fin.open("../RBF_4.csv", std::ios::in);
//
//    // Read the Data from the file as String Vector
//    std::vector<std::string> row;
//    std::string line, word, temp;
//
//        // Read an entire row and store it in a string variable 'line' - Frist row is a mean
//    int idx = 0;
//    while ( std::getline(fin, line)) {
//        row.clear();
//        //std::getline(fin, line);
//
//        // used for breaking words
//        std::stringstream s(line);
//
//        // read every column data of a row and store it in a string variable, 'word'
//        while (std::getline(s, word, ' ')) {
//            // add all the column data of a row to a vector
//            row.push_back(word);
//        }
//
//        // First values is Beta, the rest are for the centroid // convert string to double for comparision
//        char* pEnd;
//        Beta(idx) = std::strtod( row[0].c_str(), &pEnd );
//        for (int i = 1; i < row.size(); i ++){
//            Mean(idx,i-1) = std::strtod( row[i].c_str(), &pEnd );
//        }
//
//        if (idx > nRBF){
//            std::cout << "Idx > nRFB" << std::endl;
//        }
//        idx += 1;
//    }
//
//    std::cout << "Mean: \n" << Mean << std::endl;
//    std::cout << "Beta: \n" << Beta.transpose() << std::endl;
//}