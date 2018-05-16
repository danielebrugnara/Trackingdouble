#include "Event.h"


//InteractionPt class implementation//////////////////////////////////////////////////////////////////////////////



InteractionPt::InteractionPt(){

}

InteractionPt::InteractionPt(const int & segment, const double & en, const Vec3 & vec, const int & detector){
    this->segment=segment;
	this->en =en;
	this->vec=vec;
    this->detector=detector;
}



Vec3 InteractionPt::GetPosition()const{
	return vec;
}

double InteractionPt::GetEnergy()const{
	return en;
}


int InteractionPt::GetDetector()const{
	return detector;
}

int InteractionPt::GetSegment()const{
    return segment;
}

//Event class implementation//////////////////////////////////////////////////////////////


Event::Event(){}

Event::Event(std::string Line){

}

Event::Event(const InteractionPt & point){
	points.push_back(point);
}

void Event::AddInteractionPt(const InteractionPt & point){
	points.push_back(point);
}

void Event::SetEventNumber(long i){
    eventnumber=i;
}

size_t Event::NumberofInteractionPts()const{
	return points.size();
}

void Event::Clear(){
	points.clear();
}

void Event::Print(std::ostream & out )const{
    
	std::cout << "$\n";
	for (unsigned int i=0; i<points.size(); i++){
		out << points[i];
	}
}

InteractionPt Event::GetInteractionPt(const int & i)const{
	return points[i];
}

double Event::GetTotalEnergy()const{
    double energy=0;
    for (unsigned int i=0; i<points.size(); i++){
        energy+=points[i].GetEnergy();
    }
    return energy;
}
double Event::PointsDistance(const int & i, const int & j)const{
    return (points.at(i).GetPosition()-points.at(j).GetPosition()).Norm();
}

//Merge interaction points that are close together////////////////////////////////////////////////
void Event::MergePoints(std::vector<int> interaction_numbers){
    
    double etmp=points.at(interaction_numbers.at(0)).GetEnergy();
    double etot=etmp;
    Vec3 A=points.at(interaction_numbers.at(0)).GetPosition()*etmp;
    
    for (unsigned int i=1; i<interaction_numbers.size(); i++){
        etmp=points.at(interaction_numbers.at(i)-i+1).GetEnergy();
        A=A+points.at(interaction_numbers.at(i)-i+1).GetPosition()*etmp;
        etot+=etmp;
        points.erase(points.begin()+interaction_numbers.at(i)-i+1);
    }
    A=A*(1.0/etot);
    InteractionPt inttmp(points.at(interaction_numbers.at(0)).GetSegment(), etot, A, points.at(interaction_numbers.at(0)).GetDetector());
    points.at(interaction_numbers.at(0))=inttmp;
}

//operator overloading to input data
std::istream & operator >> (std::istream & in, InteractionPt & b){
	in >> b.segment >> b.en >> b.vec >> b.detector;
	return in;
}

std::ostream & operator << (std::ostream & ou, const InteractionPt & b){
    ou << b.segment <<" "<<b.en<< " "<<b.vec<< " " <<b.detector <<std::endl;
	return ou;
}
