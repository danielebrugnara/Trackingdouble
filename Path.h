#ifndef PATH_H
#define PATH_H

#include "Event.h"

class Path: public Event{
public:
    Path();
    InteractionPt At(const int & i);
    void SetEvent(const Event & ev);
    void OrderPoints(const std::vector <int> & order);
    int  GetLastPermutatedIndex();
    
private:
    
    long double meritfactor;
    std::vector<int> order;
};

#endif
