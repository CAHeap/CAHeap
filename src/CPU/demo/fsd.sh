#!/bin/sh


make testflowsizedistribution;
./testflowsizedistribution ../../../data/5s/0.dat 8000 16000 16000 4000 ../../../results/fsd/fsd.txt w
./testflowsizedistribution ../../../data/5s/0.dat 12000 16000 16000 4000 ../../../results/fsd/fsd.txt a
./testflowsizedistribution ../../../data/5s/0.dat 16000 16000 16000 4000 ../../../results/fsd/fsd.txt a
./testflowsizedistribution ../../../data/5s/0.dat 20000 16000 16000 4000 ../../../results/fsd/fsd.txt a
./testflowsizedistribution ../../../data/5s/0.dat 24000 16000 16000 4000 ../../../results/fsd/fsd.txt a
./testflowsizedistribution ../../../data/5s/0.dat 28000 16000 16000 4000 ../../../results/fsd/fsd.txt a
./testflowsizedistribution ../../../data/5s/0.dat 32000 16000 16000 4000 ../../../results/fsd/fsd.txt a

