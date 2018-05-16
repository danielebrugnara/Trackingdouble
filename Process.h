#ifndef PROCESS_H
#define PROCESS_H

#include <algorithm>

#include "Event.h"
#include "Vector.h"
#include "Path.h"

struct finalevent{
    std::vector<int> order1;
    std::vector<int> order2;
    double factor;
};

class Process{
public:
    Process();
    void LoadEvent(const Event & ev);
    void EvaluateEvent(std::ostream & out);
    void ClearAll();
    
private:
    //structs//////////////////////////////////////////
    struct meritfactor{
        int nr;
        double factor;
    };

    //methods//////////////////////////////////////////
    void MergePoints();
    double ComputeScatteringCosAngle(const int & i, const int & j, const int & k);
    double ComputeScatteringCosAngle(const int & j, const int & k);
    double ComputeComptonFactor(const int & i, const int & j, const int & k, const double & E1,  const double & E2);
        double ComputeComptonFactor(const int & i, const int & j, const double & E1,  const double & E2);
    double ComputeNishinaSigma(const int & i, const int & j, const int & k, const double & E1,  const double & E2);
    double ComputeNishinaSigma(const int & i, const int & j, const double & E1,  const double & E2);
    double ComputeNishinaSigmaTotal(const double & E);
 //   double ComputeTotalFactor(const std::vector <int> & interactionorder, std::vector <meritfactor> & meritfactors, const double & etot);
    double ComputeTotalFactor(const std::vector <int> interactionorder, std::vector <meritfactor> meritfactors, const double etot);

    finalevent ComputeDoubleProbability();
    finalevent ComputeSingleProbability();
    void Print(std::ostream & out, const int & a, const int & b);
    
    //objects//////////////////////////////////////////
    Event ev;
    Path pth;
    Path pth2;
    size_t npoints;
    std::vector<Vec3> frontfaces;
    std::vector<double> totalfactors;
    //constan objects///////////////////////////////////

};



#endif
