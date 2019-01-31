// ****************************************************************************
// Copyright		: 	NUDT.
// ============================================================================
// FILE NAME		:	AHeap_top.v
// CREATE DATE		:	2019-1-9 
// AUTHOR			:	xiongzhiting
// AUTHOR'S EMAIL	:	
// AUTHOR'S TEL		:	
// ============================================================================
// RELEASE 	HISTORY		-------------------------------------------------------
// VERSION 			DATE				AUTHOR				DESCRIPTION
// WM0.0			2019-1-9 			xiongzhiting		NS top module 
// ============================================================================
// KEYWORDS 		: 	AHeap_top
// ----------------------------------------------------------------------------
// PURPOSE 			: 	This is TOP module
// ----------------------------------------------------------------------------
// ============================================================================
// REUSE ISSUES
// Reset Strategy	:	Async clear,active low
// Clock Domains	:	clk
// Critical TiminG	:	N/A
// Instantiations	:	N/A
// Synthesizable	:	N/A
// Others			:	N/A
// ****************************************************************************
module AHeap_top(
	//clock and reset signal
	input 						SYS_CLK,					//clock, this is system clock
	input 						RESET_N,					//Reset the all signal, active low
	
	//Input port
	input						KEY_WR,						//write signal
	input	[127:0]				KEY,						//KEY
	input	[31:0]				FREQ,						//if find a same key, freq + 1
	input	[31:0]				TIMESTAMP,					//timestamp
	output						KEY_ALLMOSTFULL				//key is allmostfull
	
);
	wire	[223:0]				hash_value_input;			//KEY+FREQ+TIMESTAMP+VALUE
	wire						hash_value_input_wr;
half32_128bit	half32_128bit_inst(
//----------CLK & RST INPUT-----------
	.clk						(SYS_CLK								),			//The clock come from 
	.reset_n					(RESET_N								),			//hardware reset

//-----------CLK & RST GEN-----------
	.data						({KEY,FREQ,TIMESTAMP}					),			//Origin KEY
	.datavalid					(KEY_WR									),			//key write
	.endofpacket				(KEY_WR									),			//key write
	.checksum					(hash_value_input						),			//hash_value
	.crcvalid					(hash_value_input_wr					)			//hash_value_wr
);	
	wire	[223:0]				key1_value_input;			//KEY+FREQ+TIMESTAMP+VALUE
	wire						key1_value_input_wr;
	wire						key1_value_alf;
	wire	[223:0]				key2_value_input;			//KEY+FREQ+TIMESTAMP+VALUE
	wire						key2_value_input_wr;
	wire						key2_value_alf;
	wire	[223:0]				key3_value_input;			//KEY+FREQ+TIMESTAMP+VALUE
	wire						key3_value_input_wr;
	wire						key3_value_alf;
	wire	[223:0]				key4_value_input;			//KEY+FREQ+TIMESTAMP+VALUE
	wire						key4_value_input_wr;
	wire						key4_value_alf;
	wire	[223:0]				key5_value_input;			//KEY+FREQ+TIMESTAMP+VALUE
	wire						key5_value_input_wr;
	wire						key5_value_alf;	
Dleft#(
	.RAMAddWidth				(14										),
	.DataDepth					(16384									)
) delft0(
	//clock and reset signal
	.Clk						(SYS_CLK								),						//clock, this is synchronous clock
	.Reset_N					(RESET_N								),					//Reset the all signal, active high
	.Tab_reset_n				(RESET_N								),				//clean the table
	//Input port
	.Dleft_in_key				(hash_value_input						),				//receive metadata
	.Dleft_in_key_wr			(hash_value_input_wr					),			//receive write
	.Dleft_out_key_alf			(KEY_ALLMOSTFULL						),			//output ACL allmostfull
	//Output port
	.Dleft_out_key				(key1_value_input						),				//send metadata to DMUX
	.Dleft_out_key_wr			(key1_value_input_wr					),			//receive write to DMUX
	.Dleft_in_key_alf			(key1_value_alf							),			//output ACL allmostfull
	//ADJ table local bus
	.local_cs_n					(1'b1									),					//chip select, 0<->select, 1<->no select
	.local_rw					(1'b0									),	                //The localbus writing data  
	.local_addr					(14'b0									),	                //The localbus addr
	.local_wdata				(192'b0									),                //1<->writing, 0<->reading
	.local_rdata				(										),                //The localbud reading data
	.local_ack_n 				(										)               //Completion assert low, active low
);

Dleft#(
	.RAMAddWidth				(13										),
	.DataDepth					(8192									)
) delft1(
	//clock and reset signal
	.Clk						(SYS_CLK								),						//clock, this is synchronous clock
	.Reset_N					(RESET_N								),					//Reset the all signal, active high
	.Tab_reset_n				(RESET_N								),				//clean the table
	//Input port
	.Dleft_in_key				(key1_value_input						),				//receive metadata
	.Dleft_in_key_wr			(key1_value_input_wr					),			//receive write
	.Dleft_out_key_alf			(key1_value_alf							),			//output ACL allmostfull
	//Output port
	.Dleft_out_key				(key2_value_input						),				//send metadata to DMUX
	.Dleft_out_key_wr			(key2_value_input_wr					),			//receive write to DMUX
	.Dleft_in_key_alf			(key2_value_alf							),			//output ACL allmostfull
	//ADJ table local bus
	.local_cs_n					(1'b1									),					//chip select, 0<->select, 1<->no select
	.local_rw					(1'b0									),	                //The localbus writing data  
	.local_addr					(14'b0									),	                //The localbus addr
	.local_wdata				(192'b0									),                //1<->writing, 0<->reading
	.local_rdata				(										),                //The localbud reading data
	.local_ack_n 				(										)               //Completion assert low, active low
);

Dleft#(
	.RAMAddWidth				(12										),
	.DataDepth					(4096									)
) delft2(
	//clock and reset signal
	.Clk						(SYS_CLK								),						//clock, this is synchronous clock
	.Reset_N					(RESET_N								),					//Reset the all signal, active high
	.Tab_reset_n				(RESET_N								),				//clean the table
	//Input port
	.Dleft_in_key				(key2_value_input						),				//receive metadata
	.Dleft_in_key_wr			(key2_value_input_wr					),			//receive write
	.Dleft_out_key_alf			(key2_value_alf							),			//output ACL allmostfull
	//Output port
	.Dleft_out_key				(key3_value_input						),				//send metadata to DMUX
	.Dleft_out_key_wr			(key3_value_input_wr					),			//receive write to DMUX
	.Dleft_in_key_alf			(key3_value_alf							),			//output ACL allmostfull
	//ADJ table local bus
	.local_cs_n					(1'b1									),					//chip select, 0<->select, 1<->no select
	.local_rw					(1'b0									),	                //The localbus writing data  
	.local_addr					(14'b0									),	                //The localbus addr
	.local_wdata				(192'b0									),                //1<->writing, 0<->reading
	.local_rdata				(										),                //The localbud reading data
	.local_ack_n 				(										)               //Completion assert low, active low
);
winnertree#(
	.RAMAddWidth				(11										),
	.DataDepth					(2048									)
) winnertree0(
	//clock and reset signal
	.Clk						(SYS_CLK								),						//clock, this is synchronous clock
	.Reset_N					(RESET_N								),					//Reset the all signal, active high
	.Tab_reset_n				(RESET_N								),				//clean the table
	//Input port
	.Dleft_in_key				(key3_value_input						),				//receive metadata
	.Dleft_in_key_wr			(key3_value_input_wr					),			//receive write
	.Dleft_out_key_alf			(key3_value_alf							),			//output ACL allmostfull
	//Output port
	.Dleft_out_key				(key4_value_input						),				//send metadata to DMUX
	.Dleft_out_key_wr			(key4_value_input_wr					),			//receive write to DMUX
	.Dleft_in_key_alf			(key4_value_alf							),			//output ACL allmostfull
	//ADJ table local bus
	.local_cs_n					(1'b1									),					//chip select, 0<->select, 1<->no select
	.local_rw					(1'b0									),	                //The localbus writing data  
	.local_addr					(14'b0									),	                //The localbus addr
	.local_wdata				(192'b0									),                //1<->writing, 0<->reading
	.local_rdata				(										),                //The localbud reading data
	.local_ack_n 				(										)               //Completion assert low, active low
);
winnertree#(
	.RAMAddWidth				(10										),
	.DataDepth					(1024									)
) winnertree1(
	//clock and reset signal
	.Clk						(SYS_CLK								),						//clock, this is synchronous clock
	.Reset_N					(RESET_N								),					//Reset the all signal, active high
	.Tab_reset_n				(RESET_N								),				//clean the table
	//Input port
	.Dleft_in_key				(key4_value_input						),				//receive metadata
	.Dleft_in_key_wr			(key4_value_input_wr					),			//receive write
	.Dleft_out_key_alf			(key4_value_alf							),			//output ACL allmostfull
	//Output port
	.Dleft_out_key				(key5_value_input						),				//send metadata to DMUX
	.Dleft_out_key_wr			(key5_value_input_wr					),			//receive write to DMUX
	.Dleft_in_key_alf			(key5_value_alf							),			//output ACL allmostfull
	//ADJ table local bus
	.local_cs_n					(1'b1									),					//chip select, 0<->select, 1<->no select
	.local_rw					(1'b0									),	                //The localbus writing data  
	.local_addr					(14'b0									),	                //The localbus addr
	.local_wdata				(192'b0									),                //1<->writing, 0<->reading
	.local_rdata				(										),                //The localbud reading data
	.local_ack_n 				(										)               //Completion assert low, active low
);
winnertree#(
	.RAMAddWidth				(9										),
	.DataDepth					(512									)
) winnertree2(
	//clock and reset signal
	.Clk						(SYS_CLK								),						//clock, this is synchronous clock
	.Reset_N					(RESET_N								),					//Reset the all signal, active high
	.Tab_reset_n				(RESET_N								),				//clean the table
	//Input port
	.Dleft_in_key				(key5_value_input						),				//receive metadata
	.Dleft_in_key_wr			(key5_value_input_wr					),			//receive write
	.Dleft_out_key_alf			(key5_value_alf							),			//output ACL allmostfull
	//Output port
	.Dleft_out_key				(										),				//send metadata to DMUX
	.Dleft_out_key_wr			(										),			//receive write to DMUX
	.Dleft_in_key_alf			(1'b0									),			//output ACL allmostfull
	//ADJ table local bus
	.local_cs_n					(1'b1									),					//chip select, 0<->select, 1<->no select
	.local_rw					(1'b0									),	                //The localbus writing data  
	.local_addr					(14'b0									),	                //The localbus addr
	.local_wdata				(192'b0									),                //1<->writing, 0<->reading
	.local_rdata				(										),                //The localbud reading data
	.local_ack_n 				(										)               //Completion assert low, active low
);
endmodule