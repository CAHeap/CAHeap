#!/bin/sh


make testpersistent;

./testpersistent ../../../data/60s/0.dat 8000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt w
./testpersistent ../../../data/60s/0.dat 12000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt a
./testpersistent ../../../data/60s/0.dat 16000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt a
./testpersistent ../../../data/60s/0.dat 20000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt a
./testpersistent ../../../data/60s/0.dat 24000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt a
./testpersistent ../../../data/60s/0.dat 28000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt a
./testpersistent ../../../data/60s/0.dat 32000 20 20 1 16000 30 ../../../results/persistentflow/persistent.txt a