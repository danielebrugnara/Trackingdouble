#ifndef PROCESS_H
#define PROCESS_H

#define SQ(x) ((x)*(x))
#define CB(x) ((x)*(x)*(x))
#define ECS 661.7

#include <algorithm>
#include <math.h>

#include "Event.h"
#include "Vector.h"

//Timer to beanchmark the code/////////////////////////////////////////////////////////////////////////////////////


struct finalevent{
    std::vector<int> order1;
    std::vector<int> order2;
    double factor;
    double energy1;
    double energy2;
};

class Process{
public:
    Process();
    void LoadEvent(const Event & ev);
    void EvaluateEvent(std::ostream & out, int & number_of_single, int & number_of_double);
    void AddOriginal(const Event & ev_orig);
    void ClearAll();
    
private:
    //structs//////////////////////////////////////////
    struct meritfactor{
        int nr;
        double factor;
    };
    
    //methods//////////////////////////////////////////
    void MergePoints();
    void ComputeDistances();
    double ComputePhotoelectricSigma(double E);
    void ComputePhotoelectricSigmas();
    double ComputeScatteringCosAngle(const int & i, const int & j, const int & k);
    double ComputeScatteringCosAngle(const int & j, const int & k);
    double ComputeComptonFactor(const int & i, const int & j, const int & k, const double & E1,  const double & E2);
        double ComputeComptonFactor(const int & i, const int & j, const double & E1,  const double & E2);
    double ComputeNishinaSigma(const int & i, const int & j, const int & k, const double & E1,  const double & E2);
    double ComputeNishinaSigma(const int & i, const int & j, const double & E1,  const double & E2);
    double ComputeNishinaSigmaTotal(const double & E);
    double DistanceGe(const int &i, const int & j);
    double DistanceGe(const int &i);
    double ComputeTotalFactor(const std::vector <int> & interactionorder, std::vector <meritfactor> & meritfactors, const double & etot);
    finalevent ComputeDoubleProbability();
    finalevent ComputeSingleProbability();
    void Print(std::ostream & out, const int & a, const int & b);
    
    //objects//////////////////////////////////////////
    Event ev;
    Event ev_orig;
    std::vector<std::vector <double> > distancematr;
    std::vector<std::vector <double> > gedistancematr;
    std::vector<Vec3> frontfaces;
    double const rinner=235;//mm
    double const resolution=4;
    double const fine_alpha=0.0072993;
    double const mec2=511;//keV
    double const Z_ge=32;
    double const r0=2.81794e-12;//raggio classico elettrone
    double const NrhA=4.41348e19; //1/mm^3
    double const treashold_double=1;
    double const treashold_single=1;
    double const sigma_E=1; //2keV      Risoluzione energetica
    double const sigma_r=4; //4mm       Risoluzione posizionale
    double const pi=3.14159265359;
    size_t nrpts;
    std::vector<double> photosigma;
    std::vector<double> totalfactors;
};





#endif
