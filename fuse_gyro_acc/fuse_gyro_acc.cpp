#include "fuse_gyro_acc.h"
#define PI 3.1415926535897932384626433832795

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <math.h>
#endif

OrientationFusion::OrientationFusion(double init_pitch, double init_roll, double init_yaw){
    pitch = init_pitch;
    roll = init_roll;
    yaw = init_yaw;
    alpha = 0.98;
}

void OrientationFusion::UpdateInitOrientation(double init_pitch, double init_roll, double init_yaw){
    pitch = init_pitch;
    roll = init_roll;
    yaw = init_yaw;
}

void OrientationFusion::FuseGyroAcc(double acc[], double gyro[], double dt){
    pitch += (gyro[0] / GYROSCOPE_SENSITIVIY) * dt;
    roll += (gyro[1] / GYROSCOPE_SENSITIVIY) * dt;

    /*
        Eqt for roll pitch here:
        https://ahrs.readthedocs.io/en/latest/filters/tilt.html
        https://stackoverflow.com/questions/3755059/3d-accelerometer-calculate-the-orientation
    */
   double pitchAcc = atan2(-1 * acc[0], sqrt(acc[1] * acc[1] + acc[2] * acc[2])) * 180.0 / PI; //this is pitch from accelerometer value. This value will be in degrees
   pitch = pitch * alpha + (pitchAcc-1.5) * (1-alpha);

   double rollAcc = atan2(acc[1], acc[2]) * 180.0 / PI;
   roll = roll * alpha + (rollAcc+6.3) * (1-alpha);

   yaw += (gyro[2] / GYROSCOPE_SENSITIVIY) * dt * 180.0 / PI;
}