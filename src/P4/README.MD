# CAHeap P4 Project
***
## Environment
- `Ubuntu 16.04 LTS`

# AHeapbmv2
## Tips
If you are not familiar with P4, we recommend installing the entire p4 tutorial. You can practice how to write P4 code by doing the exercise, and build test by referencing the python code.

## How to Install
1. Install p4c compiler and several dependencies, please follow the instructions on https://github.com/p4lang/p4c. 
2. Install behavioral model and its dependencies, follow the instructions on https://github.com/p4lang/behavioral-model
3. Install Packet Testing Framework, follow the instructions on https://github.com/p4lang/ptf
Alternatively, you can install the P4 tutorial.

## How to Run
1. Compile the p4 program using command: `p4c-bm2-ss --p4v 16 --p4runtime-file {programname}.p4info --p4runtime-format text -o {programname}.json {programname}.p4`
2. Write your python script to access control plane and test your P4 program

	**Note: Since the bmv2 code of CAHeap is quite simple and straightforward, we didn't actually perform testing on behavioral model. Instead, we mainly focus on the tofino model.**

# AHeaptofino

## How to Install
1. You need to install Barefoot Networks Software Development Environment
2. Follow the instrcutions in bf-sde README file.

## How to Run
1. Compile the p4 program using command: `p4c-tofino --p4-14-spec --output=<path-to-output-file> <path-to-p4-program>`
2. Write your python script to access control plane and test your P4 program.

