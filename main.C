#include <iostream>
#include <fstream>
#include <sstream>

//Vector calculations class
#include "Vector.h"

//Event classification class
#include "Event.h"

//Processing event class
#include "Process.h"

//Classification of the line///////////////////////////////////////////////////////////////////////////////////////
enum LineType {
    BLANK_EVENT,
    SCATTERING_EVENT,
    EVENT_START,
    ORIGINAL_EVENT,
    HEADER_END
};

//Timer to beanchmark the code/////////////////////////////////////////////////////////////////////////////////////
class Timer {
private:
    unsigned long begTime;
public:
    void Start() {
        begTime = clock();
    }
    unsigned long ElapsedTime() {
        return ((unsigned long) clock() - begTime) / CLOCKS_PER_SEC;
    }
    bool IsTimeout(unsigned long seconds) {
        return seconds >= ElapsedTime();
    }
};

enum LineType Classify(std::string Line);

//MAIN////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv ){
    
    Timer timer;
    
    timer.Start();
    //External Files
    std::string Line, in_file_name="./Events/GammaEvents.0001_single";
    std::string out_file_name="./Tracked/tracked_double";
    std::ifstream infile(in_file_name);
    if (!infile.is_open()) std::cout<<"Could not open the file" << std::endl;
    std::ofstream out(out_file_name);
    
    //Vectors and points to read from the file
    Vec3 vec_tmp(0, 0, 0);
    InteractionPt point_tmp;
    InteractionPt point_orig;
    
    //Class that describes the single event
    Event event_tmp, event_orig;

    //Class to process the single event
    Process processor;
    
    //Set here the maximum number of events read and processed from the file
    const int MAX_EVENTS=5000000;
    int good_events=0;
    
    //Program start
    std::cout <<"****************************** program started ************************************"<<std::endl;
    int events_number=-1;
    bool skip=true;
    
    //Loop on file lines
    while (std::getline(infile, Line) && events_number<MAX_EVENTS) { //number of events maximum read MAX_EVENTS
        //The read line is classified
        if (skip) { //Jump Header
            if (Classify(Line)==HEADER_END)
                skip=false;
            continue;
        }
        switch (Classify(Line)){
            case EVENT_START: {//new Event (if there are  newlines the program registers more events than it should be)
                event_tmp.SetEventNumber(events_number);
                event_orig.SetEventNumber(events_number);
                events_number++;
                if (abs(event_tmp.GetTotalEnergy()-ECS)<5){ //Let's process this event!
                    std::cout <<"Analyzing a good event, nr. " <<events_number <<std::endl;
                    good_events++;
                    processor.ClearAll();
                    processor.LoadEvent(event_tmp);
                    processor.AddOriginal(event_orig);
                    processor.EvaluateEvent(out);
                }
                event_tmp.Clear();
                event_orig.Clear();
                break;
            }
            case BLANK_EVENT:{//throw Away
                break;
            }
            case SCATTERING_EVENT:{//point of interaction
                std::stringstream(Line) >> point_tmp;
                event_tmp.AddInteractionPt(point_tmp);
                break;
            }
                
            case ORIGINAL_EVENT:{//might be recorded to confront tracked with original
                std::stringstream(Line)>> point_orig;
                event_orig.AddInteractionPt(point_orig);
                break;
            }
            default:{//Just in case
                break;
            }
        }
    };
    
    //Program Finished
    std::cout <<"****************************** program ended  ************************************"<<std::endl;
    std::cout <<"Total number of events: "<<events_number <<std::endl;
    std::cout <<"Output file name: "<<out_file_name <<std::endl;
    std::cout <<"Good events: "<<good_events <<std::endl;
    std::cout <<"Total efficiency "<<(good_events*1.0)/(events_number*1.0)*100<<"%" <<std::endl;
    std::cout <<"Computing time "<< timer.ElapsedTime() <<  " seconds " << std::endl;
    std::cout <<"****************************** program ended  ************************************"<<std::endl;
    return 0;
}



enum LineType Classify(std::string Line){
    if(!Line.compare(0, 4, "-100"))
        return EVENT_START;
    if(!Line.compare(0, 5, " -101")||!Line.compare(0, 5, " -102"))
        return BLANK_EVENT;
    if (!Line.compare(0, 5, "   -1"))
        return ORIGINAL_EVENT;
    if (!Line.compare(0, 1, "$"))
        return HEADER_END;
    else
        return SCATTERING_EVENT;
}
