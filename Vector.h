#ifndef VEC3_H
#define VEC3_H
#include <cmath>
#include <iostream>

class Vec3 {
public:
    //Constructors
    Vec3(double x=0, double y=0, double z=0);
    //Coordinates
    double X() const {return x;};
    double Y() const {return y;};
    double Z() const {return z;};
    //Norm
    double Norm() const;
    //Angle Between vectors
    double AngleBetween(const Vec3 & vec);
    //Cosine of the angle between the vectors
    double CosAngleBetween(const Vec3 & vec);
    friend std::ostream & operator << (std::ostream & ou, const Vec3 & b);
    friend std::istream & operator >> (std::istream & in, Vec3 & b);
    //Sum of two vectors
    friend Vec3 operator + (const Vec3 & a, const Vec3 & b);
    //Difference of two vectors
    friend Vec3 operator - (const Vec3 & a, const Vec3 & b);
    //Change of vector sign
    friend Vec3 operator - (const Vec3 & a);
    //Scalar product
    friend double operator * (const Vec3 & a, const Vec3 & b);
    //Multiplication by scalar number
    friend Vec3 operator * (const Vec3 & a, const double & b);
    
private:
    double x, y, z;
    double norm;
};


#endif
