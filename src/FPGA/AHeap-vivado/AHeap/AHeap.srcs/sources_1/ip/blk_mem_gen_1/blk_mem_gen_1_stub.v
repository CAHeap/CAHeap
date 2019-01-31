// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2018.1 (win64) Build 2188600 Wed Apr  4 18:40:38 MDT 2018
// Date        : Mon Jan 21 18:31:59 2019
// Host        : WIN-01609101751 running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub {f:/work
//               files/AHeap-vivado/AHeap/AHeap.srcs/sources_1/ip/blk_mem_gen_1/blk_mem_gen_1_stub.v}
// Design      : blk_mem_gen_1
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z100ffg900-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* x_core_info = "blk_mem_gen_v8_4_1,Vivado 2018.1" *)
module blk_mem_gen_1(clka, ena, wea, addra, dina, douta, clkb, enb, web, addrb, 
  dinb, doutb)
/* synthesis syn_black_box black_box_pad_pin="clka,ena,wea[0:0],addra[13:0],dina[191:0],douta[191:0],clkb,enb,web[0:0],addrb[13:0],dinb[191:0],doutb[191:0]" */;
  input clka;
  input ena;
  input [0:0]wea;
  input [13:0]addra;
  input [191:0]dina;
  output [191:0]douta;
  input clkb;
  input enb;
  input [0:0]web;
  input [13:0]addrb;
  input [191:0]dinb;
  output [191:0]doutb;
endmodule
