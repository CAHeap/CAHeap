#!/bin/sh

make clean;
make;

./flowsize.sh
./heavyhitter.sh
./heavychange.sh
./fsd.sh
./entropy.sh
./cardinality.sh
./ddos.sh
./sspreader.sh
./recentflow.sh
./persistentflow.sh
./burst.sh
./flowlet.sh

