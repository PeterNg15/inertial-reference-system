#ifndef COMPLEMENTARY_H
#define COMPLEMENTARY_H

/*Combine accelerometer and gyroscope data to get more precise and accurate orientation value
Orientation will only include pitch and roll

Can't get yaw because no magnetometers
*/
class OrientationFusion{
    private:
        double alpha;
        double init_pitch;
        double init_roll;
        double init_yaw;
        const double GYROSCOPE_SENSITIVIY = 16.384; //2^15/2000;
    public:
        double pitch; //this value will be "integrated" (i.e. sum after each iteration)
        double roll; //this value will also be integrated
        double yaw; //this value will also be integrated but only by gyros

    OrientationFusion(double init_pitch, double init_roll, double init_yaw);

    void UpdateInitOrientation(double init_pitch, double init_roll, double init_yaw);

    void FuseGyroAcc(double acc[], double gyro[], double dt);
};
#endif