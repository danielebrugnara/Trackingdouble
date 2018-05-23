//All definisions set in external file for readability
#include "define.h"

//Function declarations
void EvaluateEvents();
void ReadEvents(const std::string & in_file_name);

//MAIN////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[] ){
    
    //Program start
    std::cout <<"############################################################"<<std::endl;
    std::cout <<"**************** program started ***************************"<<std::endl;

    //Global variables initialization
    events_read=0;
    events_given=0;
    events_number=-1;
    good_events=0;
    
    //Number of threads
    int number_of_threads=1;
    if (std::stoi(argv[1])>0 && std::stoi(argv[1])<25){
        number_of_threads=std::stoi(argv[1]);
    }else{
        std::cout <<"Something is wrong with the number of threads, lanching with only one"<<std::endl;
    }
    
    //Starting timer
    Timer timer;
    timer.Start();
    
    //External Files
    std::string in_file_name;
    std::string out_file_name="./Tracked/output";
    if (argc==3){
        in_file_name=argv[2];
    }else{
        std::cout<<"Required input files!";
        std::cout<<"0->file name   1->number of threads   2->inputfile"<<std::endl;
    }
    
    //Input and output streams
    std::ofstream out(out_file_name);

    
    //Managing and launching threads
    std::thread reader(ReadEvents, in_file_name);
    std::thread workers[number_of_threads];
    for (int i=0; i<number_of_threads; i++){
        workers[i]=std::thread(EvaluateEvents);
    }
    
    //Join all threads
    reader.join();
    for (int i=0; i<number_of_threads; i++){
        workers[i].join();
    }
    
    //Finish the program and return stats
    std::cout <<"***************** program ended  ************************"<<std::endl;
    std::cout <<"Total number of events: "<<events_number <<std::endl;
    std::cout <<"Input file name: "<<in_file_name <<std::endl;
    std::cout <<"Output file name: "<<out_file_name <<std::endl;
    std::cout <<"Good events: "<<good_events <<std::endl;
    std::cout <<"Total efficiency "<<(good_events*1.0)/(events_number*1.0)*100<<"%" <<std::endl;
    std::cout <<"Computing time "<< timer.ElapsedTime() <<  " seconds " << std::endl;
    std::cout <<"******************* program ended  **********************"<<std::endl;

    return 0;
}


//Worker function//////////////////////////////////////////////////////////////////////////
void EvaluateEvents(){
    Event event_tmp;
    Event event_orig;
    Process processor;
    int cnt=0;
    while (event_stream.size()>0){//problema quando parte
        std::unique_lock<std::mutex> lck(mute);
        while(event_stream.size()<1) {
            time_to_read.wait(lck);
        }
        event_tmp=event_stream.front();//reads trom queue
        event_stream.pop_front();
        event_orig=event_orig_stream.front();
        event_orig_stream.pop_front();
        cnt=events_read++;
        lck.unlock();
        processor.ClearAll();
        processor.LoadEvent(event_tmp);
        processor.AddOriginal(event_orig);
 //       processor.EvaluateEvent(out);
        
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



