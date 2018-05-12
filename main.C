#include <iostream>
#include <fstream>
#include <sstream>

//Vector calculations class
#include "Vector.h"

//Event classification class
#include "Event.h"

//Processing event class
#include "Process.h"

enum LineType {
    BLANK_EVENT,
    SCATTERING_EVENT,
    EVENT_START,
    ORIGINAL_EVENT,
    HEADER_END
};

enum LineType Classify(std::string Line);

int main(int argc, char ** argv ){
    
    //External Files
    std::string Line, in_file_name="/Users/daniele/MY_PROGAMS/Trackingdouble/Trackingdouble/GammaEvents.0002_double";
    std::string out_file_name="tracked_double";
    std::ifstream infile(in_file_name);
    if (!infile.is_open()) std::cout<<"Could not ope the file" << std::endl;
    std::ofstream out(out_file_name);
    
    //Vectors and points to read from the file
    Vec3 vec_tmp(0, 0, 0);
    InteractionPt point_tmp;
    
    //Class that describes the single event
    Event event_tmp;

    //Class to process the single event
    Process processor;
    
    //Set here the maximum number of events read and processed from the file
    const int MAX_EVENTS=500000;
    int good_events=0;
    
    //Program start
    std::cout <<"****************************** program started  ************************************"<<std::endl;
    int events_number=0;
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
            case EVENT_START: {//new Event
                event_tmp.SetEventNumber(events_number);
                events_number++;
                if (abs(event_tmp.GetTotalEnergy()-ECS)<5){ //Let's process this event!
                    std::cout <<"analizzo un buon evento, nr. " <<events_number <<std::endl;
                    good_events++;
                    event_tmp=event_tmp;
                    processor.ClearAll();
                    processor.LoadEvent(event_tmp);
                    processor.EvaluateEvent(out);
                }
                event_tmp.Clear();
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
