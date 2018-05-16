#ifndef EVENT_H
#define EVENT_H
#include "Vector.h"
#include <vector>
#include <iostream>
class InteractionPt{
	public:
        InteractionPt();
		InteractionPt(const int & segment, const double & en, const Vec3 & vec,  const int & detector);
		Vec3 GetPosition()const;
		double GetEnergy() const;
		int GetDetector()const;
        int GetSegment()const;
        friend std::istream & operator >> (std::istream & in, InteractionPt & b);
		friend std::ostream & operator << (std::ostream & ou, const InteractionPt & b);
	private:
		Vec3 vec;
		double en;
		int detector;
        int segment;

};

class Event{
	public:
		Event();
        virtual ~Event()=default;
		Event(std::string Line);
		Event(const InteractionPt & point);
		void AddInteractionPt(const InteractionPt & point);
        void SetEventNumber (long i);
		size_t NumberofInteractionPts() const;
		void Clear();
		void Print(std::ostream & out)const;
        double GetTotalEnergy() const ;
        InteractionPt GetInteractionPt(const int & i) const;
        double PointsDistance(const int & i, const int & j)const;
        void MergePoints(std::vector<int> interaction_numbers);

    protected:
        std::vector<InteractionPt> points;

	private:
        long eventnumber;
};


#endif
