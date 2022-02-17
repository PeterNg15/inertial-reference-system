/*
Name: Peter Nguyen
Language: C++
Project: Payload Data Logging for Test Launch
Date of launch: 19th Feb 2022

This code is supposed to log all the IMU data into a txt file named "data.txt"
*/

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*Variable for IMU - BNO55*/
Adafruit_BNO055 BNO1 = Adafruit_BNO055(55, 0x28); //Fix address

/*Time*/
unsigned long t0; //Used to assign start time (micro()) of each iteration

/*Store current data of each sensor; 0 index is x; 1 index is y; 2 index is z*/
float accelerometer[3];
float gyroscope[3];
float magnetometer[3];

/*Variable for SD Card Module*/
File myFile;
int pinCS = 11; // Pin 10 on Arduino Uno


/*Make sure that SD card is initialized before we read/write to it; and also able to communicate with IMU*/
void setup() {
  Serial.begin(115200);
  pinMode(pinCS, OUTPUT);
  
  // Initialize IMU
  BNO1.begin();
  delay(100);


  // SD Card Initialization
  if (SD.begin()){
    Serial.println("SD card is ready to use.");
  }else{
    Serial.println("SD card initialization failed");
    return;
  }
  
}

/*Write new data to file*/
void writeFile(){
    // Create/Open file 
  myFile = SD.open("data.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");

    /*Get Data*/
    double* accelPtr = getAcceleration(BNO1);
    double* gyroPtr = getGyroscope(BNO1);
    double* magnetPtr = getMagnetometer(BNO1);

    /*Time*/
    unsigned long dT = micros() - t0; //get change in time (since last updated)

    /*Write to file*/
    //Time
    myFile.print(dT); myFile.print(", ");
    //Acceleration
    myFile.print(accelPtr[0]); myFile.print(", ");
    myFile.print(accelPtr[1]); myFile.print(", ");
    myFile.print(accelPtr[2]); myFile.print(", ");

    //Gyro
    myFile.print(gyroPtr[0]); myFile.print(", ");
    myFile.print(gyroPtr[1]); myFile.print(", ");
    myFile.print(gyroPtr[2]); myFile.print(", ");

    //Magnetometer
    myFile.print(magnetPtr[0]); myFile.print(", ");
    myFile.print(magnetPtr[1]); myFile.print(", ");
    myFile.print(magnetPtr[2]);
    myFile.println();


    myFile.close(); // close the file
    Serial.println("Done.");
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening data.txt");
  }

  /*Update last measured*/
  updateLastUpdatedTime();
}

/*Update current time*/
void updateLastUpdatedTime(){
    t0 = micros();
}

/*For testing purposes*/
void readFile(){
  // Reading the file
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
   }
    myFile.close();
  }
  else {
    Serial.println("error opening data.txt");
  }
}

/*Get IMU data and returns it*/
double* getAcceleration(Adafruit_BNO55 sensor){
    sensors_event_t accelerometerData;
    sensor.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    sensor_event_t* event = &accelerometerData;

    static double xyz[3];
    xyz[0] = event->acceleration.x;
    xyz[1] = event->acceleration.y;
    xyz[2] = event->acceleration.z;
    return xyz;
}

double* getGyroscope(Adafruit_BNO55 sensor){
    sensors_event_t gyroscopeData;
    sensor.getEvent(&gyroscopeData, Adafruit_BNO055::VECTOR_EULER);
    sensor_event_t* event = &gyroscopeData;

    static double xyz[3];
    xyz[0] = event->orientation.x;
    xyz[1] = event->orientation.y;
    xyz[2] = event->orientation.z;
    return xyz;
}

double* getMagnetometer(Adafruit_BNO55 sensor){
    sensors_event_t magnetometerData;
    sensor.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
    sensor_event_t* event = &magnetometerData;

    static double xyz[3];
    xyz[0] = event->magnetic.x;
    xyz[1] = event->magnetic.y;
    xyz[2] = event->magnetic.z;
    return xyz;
}


void loop() {
  // empty
  writeFile();
  delay(500);
}
