#!/bin/bash
# a simple script for lazy people to compile ad run
FILESIN="
GammaEvents.0001_single
GammaEvents.0002_double
"



make clean
make
clear

for f in $FILESIN
do
	echo "Launching program, file $f"
	./my_prog ./Events/$f ./Tracked/$f &
done

echo "Finished Tracking tracked files in /Tracked/$f"
