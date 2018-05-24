
CFLAGS=-Wall -O3 
CXXFLAGS=-Wall -O3 -std=c++11 
OBJECTS=main.o Vector.o Event.o Process.o 

all: tracker

tracker: $(OBJECTS)
	$(CXX) -o tracker $(OBJECTS) -lm

Vector.o: Vector.C Vector.h
main.o: main.C Vector.h 
Event.o: Event.C Event.h
Process.o: Process.C Process.h

clean:
	rm -f *.o tracker 


