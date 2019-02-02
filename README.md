# CAHeap
***
## Introduction
Flow-level traffic measurements are fundamental for various network operations such as troubleshooting anomalous conditions. It is however very challenging to obtain accurate measurements in a nowadays data center network due to its gigantic size and extremely high link speeds that place astringent demand for the time efficiency of the underlying measurement data structure. An effective way to meet this demand is to place the data structure in the fast on-chip memory. However, limited by the size of on-chip memory,the flow capacity (i.e., the number of flows a switch can accurately record) in prior work is often not large enough to trackas many flows as we desire to. Besides, existing data structures mostly support only one or a few measurement tasks, and hence for performing all common measurement tasks, multiple data structures are needed, which requires much more on-chip memory. Therefore, ideally the data structure should be generic in the sense it can support most of common measurement tasks. In this paper, we propose CAHeap, a generic data structure that supports 16 common measurement tasks yet is space-efficient. To significantly enlarge its flow capacity, we split each flow ID into Reed-Solomon (RS) fragments, each of which is much shorter than a flow ID, and store them on different switches for later decoding. To support KV tasks which are not supported by sketches, CAHeap employs a d-left hash table, and augments it with a winner tree to keep heavy-weight flows first when the d-left insert operation finds the bucket full. CAHeap is friendly for network-wide decoding by using flow branch and rehash check. We have implemented CAHeap on five platforms and one simulator (namely P4Switch, FPGA, GPU, OVS, CPU, and ns-2)and evaluated its performance for 16 common measurement tasks. Experimental results show that CAHeap extends flow capacity by more than 2x, and achieves orders of magnitude higher accuracy. For example, the average relative error of CAHeap is 100 times smaller than FlowRadar for the task of estimating flow size.

## About this Repository

- `results`: experimental results in terms of different measurement tasks  
- `data`: traces with different time intervals (5s and 60s) for test
- `src` contains code impelementations on different platforms:
  - `CPU`: CAHeap and other algorithms implemented on CPU
  - `FPGA`: CAHeap implemented on FPGA
  - `GPU`: CAHeap implemented on GPU
  - `ns-2`: CAHeap implemented on using ns-2
  - `OVS`: CAHeap implemented on OVS
  - `P4`: CAHeap implemented on P4
- more details can be found within above six folders.

## Prerequisites
- Git Large File Storage (LFS)
- cuda 9.0
- [isa-l](https://github.com/01org/isa-l)
- g++
- SIMD instructions are involved in CAHeap to achieve higher speed, so the CPU must support AVX2 instruction set.

  **Note: Please use command `git lfs clone` to clone our project**

## How to make
- For CPU and GPU implementation, change to directory `src` and use command `./auto_make.sh` to make, then you can find executable files in the corresponding directories.
- As `ns-2`, `FPGA`, `OVS`, and `P4` implementations need specific platforms, please check `README` and try them on real devices or virtual machines.


