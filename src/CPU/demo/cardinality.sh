#!/bin/sh


make testcard;

./testcard ../../../data/5s/0.dat 8000 16000 16000 4000 ../../../results/cardinality/cardinality.txt w
./testcard ../../../data/5s/0.dat 12000 16000 16000 4000 ../../../results/cardinality/cardinality.txt a
./testcard ../../../data/5s/0.dat 16000 16000 16000 4000 ../../../results/cardinality/cardinality.txt a
./testcard ../../../data/5s/0.dat 20000 16000 16000 4000 ../../../results/cardinality/cardinality.txt a
./testcard ../../../data/5s/0.dat 24000 16000 16000 4000 ../../../results/cardinality/cardinality.txt a
./testcard ../../../data/5s/0.dat 28000 16000 16000 4000 ../../../results/cardinality/cardinality.txt a
./testcard ../../../data/5s/0.dat 32000 16000 16000 4000 ../../../results/cardinality/cardinality.txt a
