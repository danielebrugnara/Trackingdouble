#ifndef DEFINE_H
#define DEFINE_H

//Standard Libraries
#include <iostream>
#include <fstream>
#include <sstream>
//Multithreading
#include <thread>
#include <deque>
#include <mutex>

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
private:
    unsigned long begTime;

};

//Thread managing//////////////////////////////////////////////////////////////////////////////////////////////////

void EvaluateEvents();
void ReadEvents(const std::string & in_file_name);



std::condition_variable wait_reader;
std::condition_variable wait_worker;

std::mutex mutereader;
std::mutex muteprinter;

bool ready;


int events_read;
int events_given;

int events_number;
int good_events;

//Thread class/////////////////////////////////////////////////////////////////////////////////////////////////////

class Workers {
public:
    Workers(){}
    
    void Initialize(const int & number_of_threads, const std::string & in_file_name){
        workers.resize(number_of_threads);
        reader=std::thread(ReadEvents, in_file_name);
        for (auto& th : workers) th=std::thread(EvaluateEvents);
    }
    int WaitForReader(){
        if (event_orig_stream.size()>1){
            return 0;
        }else{
            return 0;
        }
    };
    int WaitForWorker(){
        if (event_orig_stream.size()<1000){
            return 0;
        }else{
            return 0;
        }
    };
    void Go(){
        std::unique_lock<std::mutex> lck(mutereader);
        ready = true;
        wait_reader.notify_all();
    }
    void Join(){
        for (auto& th : workers) th.join();
        reader.join();
    }
    std::deque<Event> event_stream;
    std::deque<Event> event_orig_stream;
private:
    std::vector<std::thread> workers;
    std::thread reader;

};

Workers workers;


//Classification of a line/////////////////////////////////////////////////////////////////
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

//Worker function//////////////////////////////////////////////////////////////////////////
void EvaluateEvents(){
    Event event_tmp;
    Event event_orig;
    Process processor;
    int cnt=0;
    
    //Wait to write something in the event stream
    std::unique_lock<std::mutex> lck(mutereader);
    while (!ready) wait_reader.wait(lck);
    
    while (workers.event_stream.size()>0){//problema quando parte
        
        workers.WaitForReader();
        
        mutereader.lock();
        event_tmp=workers.event_stream.front();//reads trom queue
        workers.event_stream.pop_front();
        event_orig=workers.event_orig_stream.front();
        workers.event_orig_stream.pop_front();
        cnt=events_read++;
        mutereader.unlock();
        
        std::cout <<"Analyzing a good event, nr. " <<event_tmp.GetEventNumber() <<std::endl;
        processor.ClearAll();
        processor.LoadEvent(event_tmp);
        processor.AddOriginal(event_orig);
        processor.EvaluateEvent();
        
        muteprinter.lock();
        //       processor.Print();
        muteprinter.unlock();
    }
    
}

//Reader function///////////////////////////////////////////////////////////////////////
void ReadEvents(const std::string & in_file_name){
    std::ifstream infile(in_file_name);
    if (!infile.is_open()) std::cout<<"Could not open the file" << std::endl;
    int events_number=-1;
    bool skip=true;
    
    std::string Line;
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
    
    //Loop on file lines
    while (std::getline(infile, Line) && events_number<MAX_EVENTS) { //number of events maximum read MAX_EVENTS
        
        //Notify worker threads to start computing

        workers.WaitForWorker();
        
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
                if (abs(event_tmp.GetTotalEnergy()-ECS)<5){ //Let's load this event in the tail
                    good_events++;
                    workers.event_stream.push_back(event_tmp);
                    workers.event_orig_stream.push_back(event_orig);
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
    }
    //Reading from file finished
}


#endif
