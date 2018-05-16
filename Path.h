#ifndef PATH_H
#define PATH_H

#define SQ(x) ((x)*(x))
#define CB(x) ((x)*(x)*(x))
#define ECS (661.7)
#define RINNER 235//mm
#define RESOLUTION 4
#define FINE_ALPHA (0.0072993)
#define MEC2 (511.0)//keV
#define Z_GE (32)
#define RO 2.81794e-12//raggio classico elettrone
#define NRHAA 4.41348e19 //1/mm^3
#define TREASHOLD 0.5
#define SIGMA_E 1 //2keV      Risoluzione energetica
#define SIGMA_R 4 //4mm       Risoluzione posizionale
#define PI 3.14159265359

#include "Event.h"

class Path{
public:
    //Methods//////////////////////////////
    Path();
    void Clear();
    InteractionPt At(const int & i);
    double ComputePhotoelectricSigma(double E);
    void ComputePhotoelectricSigmas();
    void ComputeDistances();
    double DistanceGe(const int &i, const int & j);
    double DistanceGe(const int &i);

    double IncidentEnergy(const int & i);
    double PhotoSigma(const int & i);
    double Distance(const int & i, const int & j);
    double GeDistance(const int & i, const int & j);
    void SetEvent(const Event & ev);
    double GetInEnergy(const int & i);
    double GetGeDistance(const int & i, const int & j);
    double GetDistance(const int & i, const int & j);
    double GetPhotoSigma(const int & i);
    void OrderPoints(const std::vector <int> & order, const double & etot);
    int  GetLastPermutatedIndex();
private:
    //Objects//////////////////////////////
    std::vector<double> photosigma;
    std::vector<std::vector <double> > gedistancematr;
    std::vector<std::vector <double> > distancematr;
    std::vector<double> inenergy;
    const Event* evptr;
    long double meritfactor;
    size_t nrpts;
    std::vector<int> order;
};

#endif
