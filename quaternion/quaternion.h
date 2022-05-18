#ifndef QUATERNION_H
#define QUATERNION_H

/*Idea from here:
https://github.com/carrino/Quaternion
*/

class Quaternion{
    public:
        /*a,b,c,d are quaterion [qw,qx,qy,qz]*/
        double a;
        double b;
        double c;
        double d;

        Quaternion(){a=1; b=c=d=0;}
        Quaternion(double x, double y, double z){a=0; b=x; c=y; d=z;}

        // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
        Quaternion & operator*=(const Quaternion &q);
        const Quaternion operator* (const Quaternion& q) const { return Quaternion(*this) *= q; }
        Quaternion & operator+=(const Quaternion &q);
        const Quaternion operator+(const Quaternion& q) const { return Quaternion(*this) += q; }
        Quaternion & operator*=(double scale);
        const Quaternion operator*(double scale) const { return Quaternion(*this) *= scale; }
        double norm() const;
        Quaternion & normalize();
        const Quaternion conj() const;
        double dot_product(const Quaternion& q) const;

        Quaternion & operator=(const Quaternion &rhs) {
            a = rhs.a;
            b = rhs.b;
            c = rhs.c;
            d = rhs.d;
            return *this;
        }


        //roll, pitch, yaw in euler angles (in degrees)
        static const Quaternion euler_to_quaternion(float roll, float pitch, float yaw);

        // This method takes one vector and rotates it using this Quaternion.
        // The input must be a pure vector (a == 0)
        const Quaternion rotate(const Quaternion& q) const;
};
#endif

