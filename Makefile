
CFLAGS=-Wall -O3 
CXXFLAGS=-Wall -O3 -std=c++11 
OBJECTS=main.o Vector.o Event.o Process.o 

all: my_prog

my_prog: $(OBJECTS)
	$(CXX) -o my_prog $(OBJECTS) -lm

Vector.o: Vector.C Vector.h
main.o: main.C Vector.h
Event.o: Event.C Event.h
Process.o: Process.C Process.h

clean:
	rm -f *.o


