# CAHeap
***
## Introduction
Network traffic measurement is a fundamental function for optimizing network condition and user service, and has been widely studied. Prior work focuses on the time and space
efficiency, and cross-platform implementation. To achieve efficiency in terms of time and space, one effective way is to place the data structure in the fast on-chip memory. However, limited by the size of on-chip memory, the flow capacity (i.e., the number of flows a switch can accurately record) of prior work is often not enough to track all flows. It is highly desired to achieve as large flow capacity as possible. Besides, prior work mainly focuses on one or a few tasks. To perform all common measurement tasks, using multiple data structures requires much more on-chip memory. Therefore, the data structure for measurement is desirable to be generic. In this paper, we propose CAHeap which supports 16 common measurement tasks, and significantly extends the flow capacity. To extend the flow capacity, we leverage the RS code to store the fragments of each flow ID in distributed switches. As one fragment is much smaller than a flow ID, the flow capacity of a switch naturally increases. By combining the fragments stored in multiple switches, the flow ID is able to be successfully decoded. To achieve the goal of generality, based on the d-left hash table, our CAHeap uses the winner tree to keep weighted flows first when the hash table is full. CAHeap is carefully designed to support the network-wide decoding. We implement our CAHeap on five platforms and one simulator: P4Switch, FPGA, GPU, OVS, CPU, and ns-2, and evaluate the performance on 16 common measurement tasks. All the related source code has been released on Github [6] without identity information.

## About this Repository

- `results`: experimental results in terms of different measurement tasks  
- `data`: traces with different time intervals (5s and 60s) for test
- `src` contains code impelementations on different platforms:
  - `CPU`: CAHeap and other algorithms implemented on CPU
  - `FPGA`: CAHeap implemented on FPGA
  - `GPU`: CAHeap implemented on GPU4
  - `ns-2`: CAHeap implemented on using ns-2
  - `OVS`: CAHeap implemented on OVS
  - `P4`: CAHeap implemented on P4
- more details can be found within above six folders.

## Prerequisites
- [isa-l](https://github.com/01org/isa-l)
- g++
- SIMD instructions are involved in CAHeap to achieve higher speed, so the CPU must support AVX2 instruction set.

## How to make
- For CPU and GPU implementation, change to directory `src` and use command `./auto_make` to make, then you can find executable files in the corresponding directories.
- As `ns-2`, `FPGA`, `OVS`, and `P4` implementations need specific platforms, please check `README` and try them on real devices or virtual machines.


