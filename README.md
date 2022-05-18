# Libraries for the UMass Rocket Team payload
The onboard inertial reference unit (IRU) aims to compute the launch vehicle's dead reckoning position in a 
GPS-denied environment with a 75 meter (250 ft) resolution. This github repo includes the libraries that was 
implemented in the final IRU. 

# active_status
A circle queue for storing most recent data. Its application is to store most recent values from sensors so that 
it could be averaged out.

# fuse_gyro_acc
A complementary filter is implemented to fuse gyroscope and accelerometer data together to remoive integration 
drift from the orientation data.

# homogeneous_fusion
Fusing together signals from the same sensor type. By fusing together 2 sensors of the same type, noise of the 
signal can be reduced by 29%. A modified kalman filter was used for this sensor fusion process.

# quaternion
Responsible for converting euler angles to quaternions and also rotation a point in 3D space using quaternions.

