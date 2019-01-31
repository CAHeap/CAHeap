#!/bin/sh


make testflowlet;

./testflowlet ../../../data/60s/0.dat 8000 16000 30 1 ../../../results/flowlet/flowlet.txt w
./testflowlet ../../../data/60s/0.dat 12000 16000 30 1 ../../../results/flowlet/flowlet.txt a
./testflowlet ../../../data/60s/0.dat 16000 16000 30 1 ../../../results/flowlet/flowlet.txt a
./testflowlet ../../../data/60s/0.dat 20000 16000 30 1 ../../../results/flowlet/flowlet.txt a
./testflowlet ../../../data/60s/0.dat 24000 16000 30 1 ../../../results/flowlet/flowlet.txt a
./testflowlet ../../../data/60s/0.dat 28000 16000 30 1 ../../../results/flowlet/flowlet.txt a
./testflowlet ../../../data/60s/0.dat 32000 16000 30 1 ../../../results/flowlet/flowlet.txt a
