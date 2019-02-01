#!/bin/sh


make testddos;

./testddos ../../../data/5s/0.dat 8000 16000 10 10 1 ../../../results/ddos/ddos.txt w
./testddos ../../../data/5s/0.dat 12000 16000 10 10 1 ../../../results/ddos/ddos.txt a
./testddos ../../../data/5s/0.dat 16000 16000 10 10 1 ../../../results/ddos/ddos.txt a
./testddos ../../../data/5s/0.dat 20000 16000 10 10 1 ../../../results/ddos/ddos.txt a
./testddos ../../../data/5s/0.dat 24000 16000 10 10 1 ../../../results/ddos/ddos.txt a
./testddos ../../../data/5s/0.dat 28000 16000 10 10 1 ../../../results/ddos/ddos.txt a
./testddos ../../../data/5s/0.dat 32000 16000 10 10 1 ../../../results/ddos/ddos.txt a
