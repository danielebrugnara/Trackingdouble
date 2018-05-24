#ifndef DEFINE_H
#define DEFINE_H

//Standard Libraries
#include <iostream>
#include <fstream>
#include <sstream>

//Vector calculations class
#include "Vector.h"

//Event classification class
#include "Event.h"

//Processing event class
#include "Process.h"

//Class to manage threads
#include "Workers.h"

//Classification of the line///////////////////////////////////////////////////////////////////////////////////////
enum LineType {
    BLANK_EVENT,
    SCATTERING_EVENT,
    EVENT_START,
    ORIGINAL_EVENT,
    HEADER_END
};

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

//Thread managing/////////////////////////////////////////////////////////////////////////////

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






#endif
