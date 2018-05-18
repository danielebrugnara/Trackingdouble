#include <iostream>
#include <fstream>

enum LineType {
    BLANK_EVENT,
    SCATTERING_EVENT,
    EVENT_START,
    ORIGINAL_EVENT,
    HEADER_END
};
enum LineType Classify(std::string Line);

int main(int argc, char ** argv ){
	int MAX_EVENTS=1000000;
	int events_number=0;
	std::string Line, in_file_name="/Users/daniele/MY_PROGAMS/Trackingdouble/Trackingdouble/GammaEvents.0001_single_unformatted";
	std::string out_file_name="/Users/daniele/MY_PROGAMS/Trackingdouble/Trackingdouble/GammaEvents.0001_single";
	std::ifstream infile(in_file_name);
	if (!infile.is_open()) std::cout<<"Could not ope the file" << std::endl;
	std::ofstream out(out_file_name);
	std::cout <<"****************************** program started ************************************"<<std::endl;
	bool skip=true;

	while (std::getline(infile, Line) && events_number<MAX_EVENTS) { //number of events maximum read MAX_EVENTS
		if (skip) { //Jump Header
			out<<Line<<std::endl;
			if (Classify(Line)==HEADER_END){
				skip=false;
				out<<Line<<std::endl;
			}
			continue;
		}
		switch (Classify(Line)){
			case EVENT_START: {//new Event (if there are  newlines the program registers more events than it should be)
						  break;
					  }
			case BLANK_EVENT:{//throw Away
						 break;
					 }
			case SCATTERING_EVENT:{//point of interaction
						      out<<Line<<std::endl;
						      break;
					      }

			case ORIGINAL_EVENT:{//might be recorded to confront tracked with original
						    out<<"-100          "<<events_number+1<<std::endl;
						    out<<Line<<std::endl;
						    events_number++;
						    break;
					    }
			default:{//Just in case
					break;
				}
		}
	}
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
