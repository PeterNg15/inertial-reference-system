/*
Name: Peter Nguyen
Language: C++
Project: Payload Data Logging for Test Launch
Date of creation: 19th Feb, 2022

This code is the header file to determine whether or not the rocket has launched, in-flight or has landed
*/



#ifndef _MULTISENSOR_FUSION_
#define _MULTISENSOR_FUSION_

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#endif
#include <vector>
using matrix = std::vector<std::vector<float>>;

class HomogeneousFusion{

    private:
        int count;
        float dt;
        matrix X;
        matrix P;
        matrix Q;
        matrix H;
        matrix F;
        matrix R;
    public:
        //constructor to init KF
        HomogeneousFusion();
        //Return X[0]
        float getFusedValue();
        void init_kalman(float sensor1);
        void prediction();
        void update(float sensor);
        //Returns getFusedValue() after doing the KF stages
        float fuseSensors(float sensor1, float sensor2);
};

#endif