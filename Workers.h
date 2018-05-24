#ifndef WORKER_H
#define WORKER_H

#include <string>
#include <deque>
#include <thread>
#include <mutex>


#include "Event.h"
#include "define.h"


class Workers {
public:
    Workers();
    void Initialize(const int & number_of_threads, const std::string & in_file_name);
    int WaitForReader();
    int WaitForWorker();
    void Go();
    void Join();
    
    void EvaluateEvents();
    void ReadEvents(const std::string & in_file_name);
    
    std::deque<Event> event_stream;
    std::deque<Event> event_orig_stream;
private:
    std::vector<std::thread> workers;
    std::thread reader;
    
};

#endif
