#ifndef VEC3_H
#define VEC3_H
#include <cmath>
#include <iostream>

class Vec3 {
public:
    Vec3(double x=0, double y=0, double z=0);
    double X() const {return x;};
    double Y() const {return y;};
    double Z() const {return z;};
    double Norm() const;
    double AngleBetween(const Vec3 & vec);
    double CosAngleBetween(const Vec3 & vec);
    friend std::ostream & operator << (std::ostream & ou, const Vec3 & b);
    friend std::istream & operator >> (std::istream & in, Vec3 & b);
    friend Vec3 operator + (const Vec3 & a, const Vec3 & b);
    friend Vec3 operator - (const Vec3 & a, const Vec3 & b);
    friend Vec3 operator - (const Vec3 & a);
    friend double operator * (const Vec3 & a, const Vec3 & b);
    friend Vec3 operator * (const Vec3 & a, const double & b);
    
private:
    double x, y, z;
    double norm;
};


#endif
