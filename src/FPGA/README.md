# CAHeap FPGA Project
***
## Enviroment
- `Xilinx Vivado v2018.1 （64-bit）on Windows 10 OS`



## How to Install

1. Install Xilinx Vivado v2018.1 （64-bit）Installation and Licensing according to https://www.xilinx.com/support/documentation-navigation/design-hubs/dh0013-vivado-installation-and-licensing-hub.html
2.  Unzip AHeap-vivado.rar to current directory (the dirctory path should be with English characters)

## How to Start

1. Get into the directory `..\AHeap-vivado\AHeap`

2. Open AHeap.xpr project file (it will be opened with Xilinx Vivado software)

3. Run `Flow-> Run Synthesis`, after success, run `Flow -> Run implementation`

4. The project information can be observed in "Project Summary" sub-windows, and the timing information is illustrated (Total negative slack(TNS) is -4.814 ns 

   **Note: Negative slack means the clock sycle can achieve 4.814ns, the frequency  can be 1000/4.814=207.7MHz. The value may varies after synthesis and implementation, but close to the value))**