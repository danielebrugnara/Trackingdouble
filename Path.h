#ifndef PATH_H
#define PATH_H

#include "Event.h"

class Path: public Event{
public:
    Path();
    void OrderPoints(const std::vector <int> & order);
    
private:
    Event ev;
    long double meritfactor;
};

#endif
