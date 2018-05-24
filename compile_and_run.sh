#!/bin/bash
# a simple script for lazy people to compile ad run
FILESIN="
GammaEvents.0001_single
#GammaEvents.0002_double
"



make clean
make

echo -n "Press [ENTER] to continue,...: "
read var_name

clear

for f in $FILESIN
do
	echo "Launching program, file $f"
	./tracker 1  ./Events/$f  
done

echo "Finished Tracking tracked files in /Tracked/$f"
