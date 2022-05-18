// Name: Jeffrey Matheson
// Language: Arduino
// Project: Rocketry 2021-2022 - Payload
// File: Flight_Code
// Description: The final code for the payload for the FRR launch.
// Date: 2/19/2022


#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Cardinal.h>

// Pin Values
const int pin_Buzzer = 7;   // Associate the Piezo Buzzer with DIGITAL 7


// BNO055 Objects
Adafruit_BNO055 BNO1 = Adafruit_BNO055(55, 0x28);   // BNO055 Object For The 1st BNO055 IMU
Adafruit_BNO055 BNO2 = Adafruit_BNO055(55, 0x29);   // BNO055 Object For The 2nd BNO055 IMU


// File objects associated with the BNO055 data
File BNO055_1;    // File object for the 1st BNO055 data
File BNO055_2;    // File object for the 2nd BNO055 data

// File object associated with the displacement data
File Displacement;    // File object for the displacement calculations
File headingData;


// Flag Variables
int flag_hasHeading     = 0;    // Flag for marking whether or not the heading of the rocket on the rail has been obtained
int flag_hasLaunched    = 0;   // Flag for marking whether or not the rocket has launched yet
int flag_inFlight       = 1;    // Flag for marking whether or not the rocket is in-flight
int flag_programEnded   = 0;    // Flag for marking whether or not the program was successfully ended


// Variables for data comparison
double heading;   // Variable for holding the initial compass heading of the rocket

//static double dataSet_current_BNO1[6]  = {0, 0, 0, 0, 0, 0};    // Array for holding the current set of IMU recordings from the 1st BNO055; [Ax, Ay, Az, Gz, Gy, Gz]
//static double dataSet_current_BNO2[6]  = {0, 0, 0, 0, 0, 0};    // Array for holding the current set of IMU recordings from the 2nd BNO055; [Ax, Ay, Az, Gz, Gy, Gz]
//static double dataSet_previous_BNO1[6] = {0, 0, 0, 0, 0, 0};   // Array for holding the previous set of IMU recordings from the 1st BNO055; [Ax, Ay, Az, Gz, Gy, Gz]
//static double dataSet_previous_BNO2[6] = {0, 0, 0, 0, 0, 0};   // Array for holding the previous set of IMU recordings from the 2nd BNO055; [Ax, Ay, Az, Gz, Gy, Gz]

//static double dataSet_older_BNO1[6];   // Array for holding an older set of data (e.g. a minute ago) from the 1st BNO055 for checking if the rocket is "at rest"
//static double dataSet_older_BNO2[6];   // Array for holding an older set of data (e.g. a minute ago) from the 1st BNO055 for checking if the rocket is "at rest"


// ====================================================================
// Setup & Loop
// ====================================================================

// Program Setup
void setup() {
  // Pin Setup
  pinMode(pin_Buzzer, OUTPUT);    // Set the pin associated with the Piezo Buzzer to OUTPUT


  // BNO055 Initialization
  BNO1.begin();   // Initialize the 1st BNO055
  BNO2.begin();   // Initialize the 2nd BNO055

  // BNO055 Calibration
  // ### !!! ### ADD HARDCODED VARIABLES FOR CALIBRATION

  // SD Card Initialization
  if (!SD.begin()) {    // Try to initialize the SD card (defaults to Pin 10 on the UNO and Pin 53 on the MEGA)
    while (1) {   // Loop and play a "Chirp" every 500ms for a duration of 100ms as a warning
      tone(pin_Buzzer, 4000, 100);    // Play a 4 kHz tone for 100ms
      delay(500);   // Delay for 500ms to allow for a pause
    }
  }

  SD.remove("dataIMU1.csv");    // Remove "dataIMU1.csv" from the SD Card if it already exists; needed for resetting files between runs
  SD.remove("dataIMU2.csv");    // Remove "dataIMU2.csv" from the SD Card if it already exists; needed for resetting files between runs
  SD.remove("dataDisp.csv");    // Remove "dataDisp.csv" from the SD Card if it already exists; needed for resetting files between runs
  SD.remove("dataHead.csv");    // Remove "dataHead.csv" from the SD Card if it already exists; needed for resetting files between runs

  BNO055_1 = SD.open("dataIMU1.csv", FILE_WRITE);   // Open the "dataIMU1.csv" file on the SD Card (in write mode) and associate it with the BNO055_1 file object
  BNO055_1.println("Timestamp,Accelerometer [X],Accelerometer [Y],Accelerometer [Z],Linear Accel. [X],Linear Accel. [Y],Linear Accel. [Z],Gyroscope [X],Gyroscope [Y],Gyroscope [Z],Euler [X],Euler [Y],Euler [Z]");    // Print the column headers to BNO055_1
  BNO055_2 = SD.open("dataIMU2.csv", FILE_WRITE);   // Open the "dataIMU2.csv" file on the SD Card (in write mode) and associate it with the BNO055_2 file object
  BNO055_2.println("Timestamp,Accelerometer [X],Accelerometer [Y],Accelerometer [Z],Linear Accel. [X],Linear Accel. [Y],Linear Accel. [Z],Gyroscope [X],Gyroscope [Y],Gyroscope [Z],Euler [X],Euler [Y],Euler [Z]");    // Print the column headers to BNO055_2

  Displacement = SD.open("dataDisp.csv", FILE_WRITE);   // Open the "dataDisp.csv" file on the SD Card (in write mode) and associated it with the Displacement file object
  Displacement.println("### !!! ### ADD COLUMN HEADER INFORMATION HERE!!!");
  headingData = SD.open("dataHead.csv", FILE_WRITE);
  headingData.println("Timestamp,Mag [X],Mag [Y],Mag [Z],Yaw,Cardinal String,Cardinal Int");
  

  // Indicate Successful Start Up
  buzzer_playStartTone();   // Play the "start up" tone to tell the user that the program started successfully
}

// Program Loop
void loop() {
  // Get Cardinal Heading
  while (!flag_hasHeading) {
    // ### !!! ### Need to ensure that the rocket is on the rail before the heading is read
    getCardinalHeading();
  }

  // Wait until the rocket has launched
//  while (!flag_hasLaunched) {
//    checkForLaunch();
//  }

  // Record data while the rocket is in-flight
  while (flag_inFlight) {
    unsigned long currentTimestamp = millis();
    writeToSD(BNO055_1, BNO1, currentTimestamp);
    writeToSD(BNO055_2, BNO2, currentTimestamp);

    calculateDisplacement();

    // Check to see if the rocket has landed and is "at rest"
    checkForLand();
  }

  // If the rocket is no longer in flight, end the program
  if (!flag_programEnded) {
    endProgram();
  }
}


// ====================================================================
// Helper Functions
// ====================================================================

// --------------------------------------------------------------------
// Piezo Buzzer Related Functions
// --------------------------------------------------------------------

// Function for playing a start-up tone via the Piezo Buzzer
void buzzer_playStartTone() {
  const int duration = 100;   // Variable for holding the length of time for playing the "Chirps"

  // Play 20 loops of bouncing back and forth between a 3 kHz tone and a 4 kHz tone
  for (int i = 0; i < 20; i += 1) {
    tone(pin_Buzzer, 3000, duration);   // Play a 3 kHz tone on the Piezo Buzzer for the length of duration
    delay(duration * 2);    // Delay for the length of duration * 2 to allow the tone to play on the buzzer, and for a pause afterwards
    tone(pin_Buzzer, 4000, duration);   // Play a 4 kHz tone on the Piezo Buzzer for the length of duration
    delay(duration * 2);    // Delay for the length of duration * 2 to allow the tone to play on the buzzer, and for a pause afterwards
  }
}

// --------------------------------------------------------------------
// SD Card Related Functions
// --------------------------------------------------------------------

// Function for writing information to the SD Card
void writeToSD(File fileName, Adafruit_BNO055 sensorNum, unsigned long timestamp) {
  // Timestamp
  fileName.print(timestamp);    // Print the current timestamp to the specified file
  fileName.print(",");    // Print a separator to the specified file

  double* accelPtr = get_accelerometerData(sensorNum);    // Save the accelerometer data array from the specified BNO055
  double* linAccelPtr = get_linearAccelData(sensorNum);    // Save the linear accelerometer data array from the specified BNO055
  double* gyroPtr = get_angVelocityData(sensorNum);   // Save the gyroscoped data array from the specified BNO055
  double* eulerPtr = get_eulerData(sensorNum);   // Save the gyroscoped (euler) data array from the specified BNO055

  if (fileName == BNO055_1) {
    // ### !!! ### Set dataSet_previous_BNO1 = dataSet_current_BNO1
    // ### !!! ### Set dataSet_current_BNO1 = {accelPtr[0], accelPtr[1], accelPtr[2], gyroPtr[0], gyroPtr[1], gyroPtr[2]};
  } else {
    // ### !!! ### Set dataSet_previous_BNO1 = dataSet_current_BNO1
    // ### !!! ### Set dataSet_current_BNO1 = {accelPtr[0], accelPtr[1], accelPtr[2], gyroPtr[0], gyroPtr[1], gyroPtr[2]};
  }

  // Accelerometer
  fileName.print(accelPtr[0]);    // Print the x axis accelerometer data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(accelPtr[1]);    // Print the y axis accelerometer data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(accelPtr[2]);    // Print the z axis accelerometer data to the specified file
  fileName.print(",");    // Print a separator to the specified file

  // Linear Accelerometer
  fileName.print(linAccelPtr[0]);    // Print the x axis accelerometer data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(linAccelPtr[1]);    // Print the y axis accelerometer data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(linAccelPtr[2]);    // Print the z axis accelerometer data to the specified file
  fileName.print(",");    // Print a separator to the specified file

  // Gyroscope
  fileName.print(gyroPtr[0]);   // Print the x axis gyroscope data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(gyroPtr[1]);   // Print the y axis gyroscope data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(gyroPtr[2]);   // Print the z axis gyroscope data to the specified file
  fileName.print(",");    // Print a separator to the specified file

  // Euler
  fileName.print(eulerPtr[0]);   // Print the x axis euler data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(eulerPtr[1]);   // Print the y axis euler data to the specified file
  fileName.print(",");    // Print a separator to the specified file
  fileName.print(eulerPtr[2]);   // Print the z axis euler data to the specified file
  fileName.print(",");    // Print a separator to the specified file
}

// --------------------------------------------------------------------
// Rocket State Related Functions
// --------------------------------------------------------------------

// Function for obtaining the cardinal heading of the rocket once it is on the rail
void getCardinalHeading() {
  boolean headingFound = false;
  double magX, magY;
  double* magPtr = get_magnetometerData(BNO1);   // Save the gyroscoped (euler) data array from the specified BNO055
  double calculatedYaw = 0;   // Variable for holding the value of the found heading
  int cardinal_integer;
  String cardinal_string;
  Cardinal cardinal;
  magX = magPtr[0];
  magY = magPtr[1];

  // ### !!! ### ADD CALISTA'S CODE HERE
  calculatedYaw = atan2(magY, magX)*180/M_PI;
  while(calculatedYaw < 0) {
    calculatedYaw += 360;
  }

  cardinal_integer = cardinal.getInteger(3, calculatedYaw);
  cardinal_string = cardinal.getString(3, calculatedYaw);
  
  if (headingFound) {
    heading = calculatedYaw;
    // Magnetometer
    headingData.print(magPtr[0]);   // Print the x axis euler data to the specified file
    headingData.print(",");    // Print a separator to the specified file
    headingData.print(magPtr[1]);   // Print the y axis euler data to the specified file
    headingData.print(",");    // Print a separator to the specified file
    headingData.print(magPtr[2]);   // Print the z axis euler data to the specified file
    headingData.print(",");    // Print a separator to the specified file
    headingData.print(calculatedYaw);
    headingData.print(",");    // Print a separator to the specified file
    headingData.print(cardinal_string);
    headingData.print(",");    // Print a separator to the specified file
    headingData.println(cardinal_integer);
    flag_hasHeading = 1;
  }
}

// Function for checking if the rocket has launched
void checkForLaunch() {
  boolean variablesAreDifferent = false;

  // ### !!! ### JEFF ADD CODE HERE

  if (variablesAreDifferent) {
    flag_hasLaunched = 1;
    flag_inFlight = 1;
  }
}

// Function for checking if the rocket has landed
void checkForLand() {
  boolean hasLanded = false;

  // ### !!! ### ADD PETER's CODE HERE

  if (hasLanded) {
    flag_inFlight = 0;
  }
}

// Function for ending the program (i.e. closing the SD Card files, etc)
void endProgram() {
  BNO055_1.close();   // Close the File object associated with the 1st BNO055
  BNO055_2.close();   // Close the File object associated with the 2nd BNO055
  Displacement.close();
  headingData.close();

  flag_programEnded = 1;    // Set the programEnded flag HIGH so that the function only runs once
}

// --------------------------------------------------------------------
// Displacement Related Functions
// --------------------------------------------------------------------

void calculateDisplacement() {
  // ### !!! ### ADD CALISTA'S CODE HERE
}

// --------------------------------------------------------------------
// BNO055 Related Functions
// --------------------------------------------------------------------

// Function for obtaining the accelerometer data from the specified BNO055; returns an array of the three axis values
double* get_accelerometerData(Adafruit_BNO055 sensorNum) {    // Takes in a BNO055 sensor object
  sensors_event_t accelerometerData;    // Create a sensors_event_t for holding the data event
  sensorNum.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);    // Get the specified data from the specified BNO055 event

  sensors_event_t* event = &accelerometerData;    // Save the data to a sensors_event_t* variable

  static double xyz[3];   // Create an array for holding the three axis values

  xyz[0] = event->acceleration.x;   // Save the x axis data to xyz
  xyz[1] = event->acceleration.y;   // Save the y axis data to xyz
  xyz[2] = event->acceleration.z;   // Save the z axis data to xyz

  return xyz;   // Return the array
}

// Function for obtaining the linear accelerometer data from the specified BNO055; returns an array of the three axis values
double* get_linearAccelData(Adafruit_BNO055 sensorNum) {    // Takes in a BNO055 sensor object
  sensors_event_t linearAccelData;    // Create a sensors_event_t for holding the data event
  sensorNum.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);    // Get the specified data from the specified BNO055 event

  sensors_event_t* event = &linearAccelData;    // Save the data to a sensors_event_t* variable

  static double xyz[3];   // Create an array for holding the three axis values

  xyz[0] = event->acceleration.x;   // Save the x axis data to xyz
  xyz[1] = event->acceleration.y;   // Save the y axis data to xyz
  xyz[2] = event->acceleration.z;   // Save the z axis data to xyz

  return xyz;   // Return the array
}

// Function for obtaining the gyroscope data from the specified BNO055; returns an array of the three axis values
double* get_angVelocityData(Adafruit_BNO055 sensorNum) {    // Takes in a BNO055 sensor object
  sensors_event_t angVelocityData;    // Create a sensors_event_t for holding the data event
  sensorNum.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);    // Get the specified data from the specified BNO055 event

  sensors_event_t* event = &angVelocityData;    // Save the data to a sensors_event_t* variable

  static double xyz[3];   // Create an array for holding the three axis values

  xyz[0] = event->gyro.x;    // Save the x axis data to xyz
  xyz[1] = event->gyro.y;    // Save the y axis data to xyz
  xyz[2] = event->gyro.z;    // Save the z axis data to xyz

  return xyz;   // Return the array
}

// Function for obtaining the euler angle data from the specified BNO055; returns an array of the three axis values
double* get_eulerData(Adafruit_BNO055 sensorNum) {    // Takes in a BNO055 sensor object
  sensors_event_t orientationData;    // Create a sensors_event_t for holding the data event
  sensorNum.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);    // Get the specified data from the specified BNO055 event

  sensors_event_t* event = &orientationData;    // Save the data to a sensors_event_t* variable

  static double xyz[3];   // Create an array for holding the three axis values

  xyz[0] = event->orientation.x;    // Save the x axis data to xyz
  xyz[1] = event->orientation.y;    // Save the y axis data to xyz
  xyz[2] = event->orientation.z;    // Save the z axis data to xyz

  return xyz;   // Return the array
}

// Function for obtaining the magnetometer data from the specified BNO055; returns an array of the three axis values
double* get_magnetometerData(Adafruit_BNO055 sensorNum) {   // Takes in a BNO055 sensor object
  sensors_event_t magnetometerData;   // Create a sensors_event_t for holding the data event
  sensorNum.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);    // Get the specified data from the specified BNO055 event

  sensors_event_t* event = &magnetometerData;   // Save the data to a sensors_event_t* variable

  static double xyz[3];   // Create an array for holding the three axis values

  xyz[0] = event->magnetic.x;   // Save the x axis data to xyz
  xyz[1] = event->magnetic.y;   // Save the y axis data to xyz
  xyz[2] = event->magnetic.z;   // Save the z axis data to xyz

  return xyz;   // Return the array
}