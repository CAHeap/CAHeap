# CAHeap ns-2 project
"""
## Installation
- Download [Network Simulator (NS) 2.35](https://sourceforge.net/projects/nsnam/files/latest/download)and unzip it.

  ```
  $ tar -zxvf ns-allinone-2.35.tar.gz
  ```
- Copy [CAHeap.patch](https://github.com/CAHeap/CAHeap/tree/master/src/ns-2/CAHeap.patch) to the *top ns-2.35 folder* (`ns-allinone-2.35`) and apply the patch. Then install NS2
  ```
  $ cd ns-allinone-2.35
  $ patch -p1 --ignore-whitespace -i CAHeap.patch
  $ ./install
  ```
- Install  [Intel(R) Intelligent Storage Acceleration Library](https://github.com/01org/isa-l)

  Add  -lisal` to `ns-allinone-2.35/ns-2.35/Makefile

  ```
  LIB   = ...
  		-lsal\
  ```

- Install [Boost Serialize ](https://www.boost.org/)

  Add  Boost Serialize ` to `ns-allinone-2.35/ns-2.35/Makefile

  ```
  INCLUDE= ...
   		-I YourPath/boost_1_69_0 
  LIB   = ...
  		-LYourPath/boost_1_69_0 /stage/lib -lboost_serialization\
  ```

- Copy files in [common](https://github.com/CAHeap/CAHeap/tree/master/src/ns-2/common) folder to ```ns-allinone-2.35/ns-2.35/common/```.

  Add Common/ECtreap.o` to `ns-allinone-2.35/ns-2.35/Makefile
    ```
    OBJ_CC =....
		....
		common/ECtreap.o\
    ```

- Run ```make``` on ```/ns-allinone-2.34/ns-2.34```.


## Simulation
- In the tcl file [8PodFatTree.tcl](https://github.com/CAHeap/CAHeap/tree/master/src/ns-2/8PodFatTree.tcl) , we build a k=8 fat-tree topology and simulate the Loop/Blackhole. 

  ```
  ns 8PodFatTree.tcl
  ```

## Analyze Results

- In  [common](https://github.com/CAHeap/CAHeap/tree/master/src/ns-2/common) folder, the file  [calres.cc](https://github.com/CAHeap/CAHeap/tree/master/src/ns-2/common/calres.cc) is used to analyze results.
  ```
  g++ calres.cc -std=c++11 -LYourPath/boost_1_69_0/stage/lib -	          lboost_serialization -I YourPath/boost_1_69_0 -lisal -O3
  ./a.out 0
  ```

