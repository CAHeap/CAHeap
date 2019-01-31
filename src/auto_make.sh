#!/bin/bash

cd CPU/demo
make
cd ../../GPU
./test.sh
