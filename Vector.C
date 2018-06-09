#include "Vector.h"

Vec3::Vec3(double x, double y, double z){
	this->x=x;
	this->y=y;
	this->z=z;
}

double Vec3::Norm() const{
    return sqrt(x*x+y*y+z*z);
}

double Vec3::AngleBetween(const Vec3 & vec){
	return  acos((*this *vec )/(vec.Norm() *Norm()));
}

double Vec3::CosAngleBetween(const Vec3 & vec){
	return  (*this *vec )/(vec.Norm() *Norm());
}

Vec3 operator + (const Vec3 & a, const Vec3 & b){
	Vec3 c(a.x+b.x, a.y+b.y, a.z+b.z);
	return c;
};

Vec3 operator - (const Vec3 & a, const Vec3 & b){
	Vec3 c(a.x-b.x, a.y-b.y, a.z-b.z);
	return c;
};

Vec3 operator - (const Vec3 & a){
    Vec3 c(-a.x, -a.y, -a.z);
    return c;
};

double operator * (const Vec3 & a, const Vec3 & b){
	double c=a.x*b.x+a.y*b.y+a.z*b.z;
	return c;
}

Vec3 operator * (const Vec3 & a, const double & b){
	Vec3 c(a.x*b, a.y*b, a.z*b);
	return c;
}

std::ostream & operator << (std::ostream & ou, const Vec3 & b){
	ou << b.x <<" " << b.y << " " << b.z << " ";
	return ou;
}

std::istream & operator >> (std::istream & in, Vec3 & b){
	in >> b.x >> b.y >> b.z;
	return in;
}
