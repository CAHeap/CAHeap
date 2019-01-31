#!/bin/sh


make testspreader;

./testspreader ../../../data/5s/0.dat 8000 16000 10 10 1 ../../../results/sspreader/spreader.txt w
./testspreader ../../../data/5s/0.dat 12000 16000 10 10 1 ../../../results/sspreader/spreader.txt a
./testspreader ../../../data/5s/0.dat 16000 16000 10 10 1 ../../../results/sspreader/spreader.txt a
./testspreader ../../../data/5s/0.dat 20000 16000 10 10 1 ../../../results/sspreader/spreader.txt a
./testspreader ../../../data/5s/0.dat 24000 16000 10 10 1 ../../../results/sspreader/spreader.txt a
./testspreader ../../../data/5s/0.dat 28000 16000 10 10 1 ../../../results/sspreader/spreader.txt a
./testspreader ../../../data/5s/0.dat 32000 16000 10 10 1 ../../../results/sspreader/spreader.txt a