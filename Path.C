#include "Path.h"

Path::Path(){
    
}

InteractionPt Path::At(const int & i){
    return ev.GetInteractionPt(order.at(i));
}



void Path::SetEvent(const Event & ev){
    this->ev=ev;
};

void Path::OrderPoints(const std::vector <int> & order){
    this->order=order;
};



