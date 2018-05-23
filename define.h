#ifndef DEFINE_H
#define DEFINE_H

//Standard Libraries
#include <iostream>
#include <fstream>
#include <sstream>
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


//Thread managing//////////////////////////////////////////////////////////////////////////////////////////////////

std::deque<Event> event_stream;
std::deque<Event> event_orig_stream;

std::mutex mute;
std::mutex muteprinter;
std::condition_variable time_to_read;
int events_read;
int events_given;

int events_number;
int good_events;

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

#endif
