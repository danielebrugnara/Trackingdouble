#include "Workers.h"

Workers::Workers(){}

void Workers::Initialize(const int & number_of_threads, const std::string & in_file_name){
    workers.resize(number_of_threads);
    reader=std::thread(ReadEvents, in_file_name);
    for (auto& th : workers) th=std::thread(EvaluateEvents);
}
int Workers::WaitForReader(){
    if (event_orig_stream.size()>1){
        return 0;
    }else{
        return 0;
    }
};
int Workers::WaitForWorker(){
    if (event_orig_stream.size()<1000){
        return 0;
    }else{
        return 0;
    }
};
void Workers::Go(){
    std::unique_lock<std::mutex> lck(mutereader);
    ready = true;
    wait_reader.notify_all();
}
void Workers::Join(){
    for (auto& th : workers) th.join();
    reader.join();
}


//Worker function//////////////////////////////////////////////////////////////////////////
void Workers::EvaluateEvents(){
    Event event_tmp;
    Event event_orig;
    Process processor;
    int cnt=0;
    
    //Wait to write something in the event stream
    std::unique_lock<std::mutex> lck(mutereader);
    while (!ready) wait_reader.wait(lck);
    
    while (event_stream.size()>0){//problema quando parte
        
        WaitForReader();
        
        mutereader.lock();
        event_tmp=event_stream.front();//reads trom queue
        event_stream.pop_front();
        event_orig=event_orig_stream.front();
        event_orig_stream.pop_front();
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
void Workers::ReadEvents(const std::string & in_file_name){
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
        
        WaitForWorker();
        
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
                    event_stream.push_back(event_tmp);
                    event_orig_stream.push_back(event_orig);
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
