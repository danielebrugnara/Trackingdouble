//All definisions set in external file for readability
#include "define.h"


Workers workers;

//MAIN////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[] ){
    
    //Starting timer
    Timer timer;
    timer.Start();
    
    //Program start
    std::cout <<"############################################################"<<std::endl;
    std::cout <<"**************** program started ***************************"<<std::endl;

    //Global variables initialization
    events_read=0;
    events_given=0;
    events_number=-1;
    good_events=0;
    ready=false;
    
    //Number of threads
    int number_of_threads=1;
    if (std::stoi(argv[1])>0 && std::stoi(argv[1])<25){
        number_of_threads=std::stoi(argv[1]);
    }else{
        std::cout <<"Something is wrong with the number of threads, lanching with only one"<<std::endl;
    }
    
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
    workers.Initialize(number_of_threads, in_file_name);
    
    //Start workers after the reader has written some stream
    workers.Go();
    
    //Join all threads
    workers.Join();
    
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









