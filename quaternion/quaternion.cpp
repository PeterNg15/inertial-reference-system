#include "Quaternion.h"
#include <iostream>

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <math.h>
#endif

#define PI 3.1415926535897932384626433832795

Quaternion & Quaternion::operator*=(const Quaternion &q) {
    Quaternion ret;
    ret.a = a*q.a - b*q.b - c*q.c - d*q.d;
    ret.b = b*q.a + a*q.b + c*q.d - d*q.c;
    ret.c = a*q.c - b*q.d + c*q.a + d*q.b;
    ret.d = a*q.d + b*q.c - c*q.b + d*q.a;
    return (*this = ret);
}

Quaternion & Quaternion::operator+=(const Quaternion &q) {
    a += q.a;
    b += q.b;
    c += q.c;
    d += q.d;
    return *this;
}

Quaternion & Quaternion::operator*=(float scale) {
    a *= scale;
    b *= scale;
    c *= scale;
    d *= scale;
    return *this;
}


float Quaternion::norm() const {
    float norm2 = a*a + b*b + c*c + d*d;
    return sqrt(norm2);
}

Quaternion & Quaternion::normalize() {
    float n = norm();
    a /= n;
    b /= n;
    c /= n;
    d /= n;
    return *this;
}
/*roll: x; pitch: y; yaw: z*/
/*Convert euler to quaternion input is roll/pitch/yaw*/
const Quaternion Quaternion::euler_to_quaternion(double roll, double pitch, double yaw) {
    //Since roll/yaw/pitch is in euler convert to rad
    double phi = roll * PI/180;
    double theta = pitch * PI/180;
    double psi = yaw * PI/180;

    //Using the following eqt from:
    //https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    //Their theta = pitch; phi=roll; psi=yaw

    double phi_c  = cos(phi/2.00);
    double phi_s = sin(phi/2.00);
    double theta_c = cos(theta/2.00);
    double theta_s = sin(theta/2.00);
    double psi_c = cos(psi/2.00);
    double psi_s = sin(psi/2.00);

    Quaternion out;
    out.a = (phi_c*theta_c*psi_c) + (phi_s*theta_s*psi_s);
    out.b = (phi_s*theta_c*psi_c) - (phi_c*theta_s*psi_s);
    out.c = (phi_c*theta_s*psi_c) + (phi_s*theta_c*psi_s);
    out.d = (phi_c*theta_c*psi_s) - (phi_s*theta_s*psi_c);
    return out;
}

/*Inverse of the quaternion*/
const Quaternion Quaternion::conj() const {
    Quaternion ret(*this);
    ret.b *= -1;
    ret.c *= -1;
    ret.d *= -1;
    return ret;
}

float Quaternion::dot_product(const Quaternion& q) const {
    return a * q.a + b * q.b + c * q.c + d * q.d;
}

// This will roate the input vector by this normalized rotation quaternion.
const Quaternion Quaternion::rotate(const Quaternion& q) const {
    return (*this) * q * conj();
}

int main(){
    Quaternion quat;
    quat = quat.euler_to_quaternion(0.0, 45.0, 0.0);
    auto q = quat.rotate(Quaternion(0,0,1));
    //std::cout << "w:" << quat.a;
    printf("w: %f\n", q.a);
    printf("x: %f\n", q.b);
    printf("y: %f\n", q.c);
    printf("z: %f\n", q.d);
    return 0;
}