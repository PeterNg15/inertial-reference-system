/*
Name: Peter Nguyen
Language: C++
Project: Payload Data Logging for Test Launch
Date of creation: 19th Feb, 2022

This code is the header file to determine whether or not the rocket has launched, in-flight or has landed
*/



#ifndef ACTIVE_STATUS
#define ACTIVE_STATUS
//#include<Arduino.h>

class ActiveStatus{

    private:
        int memorySize;
        //Pointer to arrays that will store the memories (n most recent events)
        //Order of array is not important since we're going to average out the past n events
        double* memoryXPtr;
        double* memoryYPtr;
        double* memoryZPtr;
        //This value is to keep track of where in the memory we are; should be between 0 to n-1 (the index of the arrays)
        int currentIndex;
        //These values are used to determine if rocket is stationary and to what extent is the error acceptable
        double stationaryX;
        double stationaryY;
        double stationaryZ;
        double pm; //to see if average in range for the X, Y, Z (it is plus minus +-)
        //This value will indicate the current status of rocket. It is used for logic (and to avoid thinking the rocket has landed before it was even launched
        short int flightStatus; //0 is has not launched, 1 is in-flight, 2 is landed
    public:
        //constructor to init memory of past n events
        ActiveStatus(double startX, double startY, double startZ, double plusMinus);
        
        //This is how we'll determine if rocket has landed;
        //0 means it has not launched; 1 means it's in-flight; 2 means it has landed
        short int getStatus();

        //This will add new event to the memory as well as deleting old memory; The purpose of this is to only get the n most recent events; This will not add to the array it'll only replace the current index with the new data and then increase the current index
        void addToMemory(double newX, double newY, double newZ);

        //It'll determine if the average past events for all 3 axis is within range; Return 1 if true; else 0
        short int withinRange(double val, double target);

        //It'll update flight status based on current flight status; And then returns the current status
        //It'll invoke the addToMemory func, withinRange, and getSTatus to determine current status
        short int updateStatus(double newX, double newY, double newZ);

        double avgArray(double* arr);

        /*TESTING PURPOSES*/
        double getCurrX();
        double* getArrX();
};

#endif