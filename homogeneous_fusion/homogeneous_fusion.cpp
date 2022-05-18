
#include "homogeneous_fusion.h"
#include "UMassRocketNumpy.cpp"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

HomogeneousFusion::HomogeneousFusion(){
    //Initiailize all the arrays
    dt = 0.1; //In seconds (i.e. 100ms)
    //System noise matrix Q
    Q = matrix {{0.05,0},{0,0.05}};
    //Observation matrix H
    H = matrix {{1},{0}};
    //Transition matrix F
    F = matrix {{1, dt},{0,1}};
    //Update noise matrix R
    R = matrix {{0.056, 0}};
    //To check if initialized KF yet
    count = 0;
}

float HomogeneousFusion::getFusedValue(){
    return X.at(0).at(0);
}

void HomogeneousFusion::init_kalman(float sensor1){
    X = matrix {{sensor1}, {0.0}};
    //Covariance matrix
    P = matrix {{100.0, 0.0}, {0.0, 300.0}};
    count = 1;
}

void HomogeneousFusion::prediction(){
    X = matmul(F, X); //X = F x X
    P = matmul(F, P);
    matrix _F = transpose(F);
    P = matmul(P, _F); //P = F x P x F_t
    P = P + Q;
}

void HomogeneousFusion::update(float sensor){
    matrix _H = transpose(H);
    /*Updating matrix S*/
    matrix Inn = matmul(_H, X); //Dimension (1,1)
        //Turn double into another matrix of the same size as Inn
    matrix sensorSquare = numToSquareMatrix(sensor, Inn); //dimension (1,1)
    Inn = sensorSquare - Inn; //Dimension (1,1)
    matrix K_denom = matmul(_H, P);
    K_denom = matmul(K_denom, H);
    matrix _S = numToSquareMatrix(K_denom.at(0).at(0), R); 
    //S is Denominator of Kalman gain
    //_S = H x P x H_t + R
    K_denom = _S + R; //should have dimensions (1,2)
    
    /*Updating matrix K (kalman gain)*/
    matrix K_num = matmul(P, H);
    K_num = transpose(K_num);
    matrix K = K_num / K_denom; //This is the kalman gain matrix (1,2);
    K = transpose(K); //K gain is now (2,1)
    matrix _Inn = numToSquareMatrix(Inn.at(0).at(0), K);
    Inn = K * _Inn;
    X = X + Inn; // X = X + K * (Sensor - HX) with dimensions (2,1)
    
    /*Calculating P*/
    matrix _P = matmul(K, _H);
    _P = matmul(_P, P);
    P = P - _P; //(I - KH)P with dimensions (2,2)
}

//Procedure that does everything (1 KF iteration)
float HomogeneousFusion::fuseSensors(float sensor1, float sensor2){
    if(count == 0){
        //Set first sensor (update X and P)
        init_kalman(sensor1);
    }else{
        prediction();
        update(sensor1); //Sensor 1
        update(sensor2); //Sensor 2
    }
    return X.at(0).at(0); //Fused value
}

// int main(){
//     MultisensorFusion kf;
//     double fusedVal = kf.fuseSensors(1.00, 1.00);
//     printf("%F\n", fusedVal);
//     fusedVal = kf.fuseSensors(15.00, 15.00);
//     printf("%F\n", fusedVal);
//     fusedVal = kf.fuseSensors(15.00, 15.00);
//     printf("%F\n", fusedVal);
//     fusedVal = kf.fuseSensors(15.00, 15.00);
//     printf("%F\n", fusedVal);
//     fusedVal = kf.fuseSensors(15.00, 15.00);
//     printf("%F\n", fusedVal);
//     return EXIT_SUCCESS;
// }