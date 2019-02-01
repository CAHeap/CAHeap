#!/bin/sh

make testflowsize;
./testflowsize ../../../data/5s/0.dat 16000 8000 32000 4000 ../../../results/flowsize/flowsize.txt w
