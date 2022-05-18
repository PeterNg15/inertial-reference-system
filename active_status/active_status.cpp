/*
Name: Peter Nguyen
Language: C++
Project: Payload Data Logging for Test Launch
Date of creation: 19th Feb, 2022

This code is supposed to determine whether or not the rocket has launched, in-flight or has landed
*/

#include "active_status.h"
#include <iostream>
using namespace std;

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <string>
#endif

const int MEMORY_SIZE = 5;

ActiveStatus::ActiveStatus(double startX, double startY, double startZ, double plusMinus){
    /*Initialize the 3 arrays*/    
    static double xMem[MEMORY_SIZE] = {};
    static double yMem[MEMORY_SIZE] = {};
    static double zMem[MEMORY_SIZE] = {};
    for(int i=0; i < MEMORY_SIZE; i++){
        xMem[i] = startX;
        yMem[i] = startY;
        zMem[i] = startZ;
    }
    this->memoryXPtr = xMem;
    this->memoryYPtr = yMem;
    this->memoryZPtr = zMem;

    /*Initialize stationary value variables*/
    this->stationaryX = startX;
    this->stationaryY = startY;
    this->stationaryZ = startZ;
    this->pm = plusMinus;
    this->flightStatus = 0;
    
    /*Other*/
    this->currentIndex = 0;
    this->memorySize = MEMORY_SIZE;
}

short int ActiveStatus::getStatus(){
    return this->flightStatus;
}


double ActiveStatus::avgArray(double* arr){
    double sum = 0.0;
    for(int i=0; i<MEMORY_SIZE; i++){
        sum += arr[i];
    }
    return (double) sum / (double)MEMORY_SIZE;
}

void ActiveStatus::addToMemory(double newX, double newY, double newZ){
    int currIndex = this->currentIndex;
    //Replace value of curr index with new value
    this->memoryXPtr[currIndex] = newX;
    this->memoryYPtr[currIndex] = newY;
    this->memoryZPtr[currIndex] = newZ;

    if(currIndex < MEMORY_SIZE-1){// when index is between first index and 2nd last index
        this->currentIndex += 1;
    }else if(currIndex == MEMORY_SIZE-1){//wheni ndex is last index;
        this->currentIndex = 0;
    }
}

short int ActiveStatus::withinRange(double value, double target){
    if(value >= target-(this->pm) && value <= target+(this->pm)){
        return 1;
    }
    return 0;
}

short int ActiveStatus::updateStatus(double newX, double newY, double newZ){
    this->addToMemory(newX, newY, newZ);
    double avgX = this->avgArray(this->memoryXPtr);
    double avgY = this->avgArray(this->memoryYPtr);
    double avgZ = this->avgArray(this->memoryZPtr);
    if(withinRange(avgX, this->stationaryX) 
    && withinRange(avgY, this->stationaryY) 
    && withinRange(avgZ, this->stationaryZ)){
        //This means that it is stationary
        if(this->flightStatus == 0){
            return 0; //it has always been stationary
        }
        this->flightStatus = 2; 
        return 2;
    }
    this->flightStatus = 1;
    return 1; //This means that it is not stationary
}


/*These are for testing*/
double ActiveStatus::getCurrX(){
    return memoryXPtr[currentIndex+1];
}

double* ActiveStatus::getArrX(){
    return this->memoryXPtr;
}


/* COMMENT THIS OUT WHEN YOU USE WITH ARDUINO CODE*/
/*int main(){
    ActiveStatus status(0, 0, 0, 0.4);
    short int stat = status.updateStatus(1,1,1);
    stat = status.updateStatus(1,1,1);
    stat = status.updateStatus(1,1,1);
    stat = status.updateStatus(0,0,0);
    stat = status.updateStatus(0,0,0);
    string s = to_string(status.avgArray(status.getArrX()));
    string ss = to_string(stat);
    cout << s << ", " << ss;
    return 0;
}*/