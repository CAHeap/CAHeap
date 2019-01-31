#!/bin/sh


make testheavychange;
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 8000 16000 50 50 1 ../../../results/heavychange/heavychange.txt w
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 12000 16000 50 50 1 ../../../results/heavychange/heavychange.txt a
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 16000 16000 50 50 1 ../../../results/heavychange/heavychange.txt a
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 20000 16000 50 50 1 ../../../results/heavychange/heavychange.txt a
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 24000 16000 50 50 1 ../../../results/heavychange/heavychange.txt a
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 28000 16000 50 50 1 ../../../results/heavychange/heavychange.txt a
./testheavychange ../../../data/5s/0.dat ../../../data/5s/1.dat 32000 16000 50 50 1 ../../../results/heavychange/heavychange.txt a