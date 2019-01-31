#!/bin/sh


make testheavyhitter;
./testheavyhitter ../../../data/5s/0.dat 8000 16000 6 6 1 ../../../results/heavyhitter/heavyhitter.txt w
./testheavyhitter ../../../data/5s/0.dat 12000 16000 6 6 1 ../../../resultsheavyhitter/heavyhitter.txt a
./testheavyhitter ../../../data/5s/0.dat 16000 16000 6 6 1 ../../../results/heavyhitter/heavyhitter.txt a
./testheavyhitter ../../../data/5s/0.dat 20000 16000 6 6 1 ../../../results/heavyhitter/heavyhitter.txt a
./testheavyhitter ../../../data/5s/0.dat 24000 16000 6 6 1 ../../../results/heavyhitter/heavyhitter.txt a
./testheavyhitter ../../../data/5s/0.dat 28000 16000 6 6 1 ../../../results/heavyhitter/heavyhitter.txt a
./testheavyhitter ../../../data/5s/0.dat 32000 16000 6 6 1 ../../../results/heavyhitter/heavyhitter.txt a