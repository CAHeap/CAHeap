// ****************************************************************************
// Copyright		: 	NUDT.
// ============================================================================
// FILE NAME		:	Dleft.v
// CREATE DATE		:	2019-1-9 
// AUTHOR			:	xiongzhiting
// AUTHOR'S EMAIL	:	
// AUTHOR'S TEL		:	
// ============================================================================
// RELEASE 	HISTORY		-------------------------------------------------------
// VERSION 			DATE				AUTHOR				DESCRIPTION
// WM0.0			2019-1-9 			xiongzhiting		Replace DMAC
// ============================================================================
// KEYWORDS 		: 	Dleft
// ----------------------------------------------------------------------------
// PURPOSE 			: 	
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
module winnertree
#(parameter
	RAMAddWidth						= 14,						//The RAM address width
	DataDepth						= 16384						//The RAM data depth
)
(
	//clock and reset signal
	input 							Clk,						//clock, this is synchronous clock
	input 							Reset_N,					//Reset the all signal, active high
	input							Tab_reset_n,				//clean the table
	//Input port
	input		[223:0]				Dleft_in_key,				//receive metadata
	input							Dleft_in_key_wr,			//receive write
	output							Dleft_out_key_alf,			//output ACL allmostfull
	//Output port
	output	reg	[223:0]				Dleft_out_key,				//send metadata to DMUX
	output	reg						Dleft_out_key_wr,			//receive write to DMUX
	input							Dleft_in_key_alf,			//output ACL allmostfull
	//ADJ table local bus
	input							local_cs_n,					//chip select, 0<->select, 1<->no select
	input							local_rw,	                //The localbus writing data  
	input		[RAMAddWidth-1:0]	local_addr,	                //The localbus addr
	input		[191:0]				local_wdata,                //1<->writing, 0<->reading
	output	reg	[191:0]				local_rdata,                //The localbud reading data
	output	reg						local_ack_n                //Completion assert low, active low

);
//wire and register

	//wire
	wire							check_key_empty;			//fifo empty
	wire		[3:0]				check_key_usedw;			//fifo usedword
	wire							table_reset_n;				//table clean
	reg								check_key_rd;				//fifo read 
	wire		[223:0]				check_key_q;				//fifo data
	reg			[2:0]				count;						//count the read-signal
	reg			[223:0]				check_key_q_reg0;			//restore the key
	reg			[223:0]				check_key_q_reg1;			//restore the key
	reg			[223:0]				check_key_q_reg2;			//restore the key
	//----------------------ram--------------------------//
	reg			[RAMAddWidth-1:0]	ram_reg_addr;				//ram address register
	reg								ram_addr_rd;				//ram read singal
	reg			[RAMAddWidth-1:0]	ram_addr_a;					//ram a-port address
	reg			[RAMAddWidth-1:0]	ram_addr_b;					//ram b-port address
	reg			[191:0]				ram_data_a;					//ram a-port data
	reg			[191:0]				ram_data_b;					//ram b-port data
	reg								ram_rden_a;					//read a-port 
	reg								ram_rden_b;					//read b-port 
	reg								ram_wren_a;					//write a-port
	reg								ram_wren_b;					//write b-port
	wire		[191:0]				ram_out_a;					//data a-port
	wire		[191:0]				ram_out_b;					//data b-port
//-------------------state------------------------//
	reg			[3:0]				lookup_state;				//look up state
	parameter						idle_s 		=	4'h0,
									pharse_1_s	=	4'h1,
									pharse_2_s	=	4'h2,
									pharse_3_s	=	4'h3,
									pharse_4_s	=	4'h4,
									wait_1_s	=	4'h5,
									wait_2_s	=	4'h6,
									read_s		=	4'h7;
									
	reg			[3:0]				localbus_state;				//localbus state
	parameter						idle_d 		=	4'h0,
									read_d		=	4'h1,
									write_d		=	4'h2,
									wait_1_d	=	4'h3,
									wait_2_d	=	4'h4,
									rec_data_d	=	4'h5,
									wait_cs_d	=	4'h6,
									inital_d	=	4'h7;
	assign table_reset_n 						= Tab_reset_n & Reset_N;				//Tab_reset_n is software clean, Reset_N is hardward clean
	assign Dleft_out_key_alf					= &check_key_usedw[3:0];					//send out allmostfull
//local bus control the ram
always @ (posedge Clk or negedge table_reset_n)
begin
    if (~table_reset_n)																	//reset is coming
    begin
		local_ack_n							<= 1'b1;									//clean all signal								
		local_rdata							<= 192'b0;									//clean all signal
		ram_addr_b							<= {(RAMAddWidth){1'b0}};					//clean all signal
		ram_data_b							<= 192'b0;									//clean all signal
		ram_rden_b							<= 1'b0;									//clean all signal
		ram_wren_b							<= 1'b0;									//clean all signal
		localbus_state						<= idle_d;									//clean all signal
    end
    else 
    begin
		case(localbus_state)
			idle_d:																		//start state
			begin
				local_ack_n					<= 1'b1;									//clean all signal
				ram_addr_b					<= {(RAMAddWidth){1'b0}};					//clean all signal
				ram_data_b					<= 192'b0;									//clean all signal
				ram_rden_b					<= 1'b0;									//clean all signal
				ram_wren_b					<= 1'b0;									//clean all signal
				if(local_cs_n == 1'b0)													//local bus chip select
				begin
					if(local_rw == 1'b1)												//1: wirte, 0:read
						localbus_state		<= write_d;									//goto write
					else
						localbus_state		<= read_d;									//goto read
				end
				else
				begin
					localbus_state			<= idle_d;									//waiting cs
				end
			end
			write_d:																	//write data to ram.
			begin
				ram_addr_b					<= local_addr;								//send write-address
				ram_data_b					<= local_wdata;								//send write-data
				ram_wren_b					<= 1'b1;									//send write-signal
				local_ack_n					<= 1'b0;									//assert ack
				localbus_state				<= wait_cs_d;								//goto waiting
			end
			read_d: begin																//read control signal, send to ram
				ram_addr_b					<= local_addr;								//send read-address
				ram_rden_b					<= 1'b1;									//send read-signal
				localbus_state				<= wait_1_d;								//waiting read-data
			end
			wait_1_d: begin																//wait the ram
				ram_rden_b					<= 1'b0;									//clean all signal
				localbus_state				<= wait_2_d;								//waiting read-data
			end
			wait_2_d: begin																//wait the ram
				localbus_state				<= rec_data_d;								//waiting read-data
			end
			rec_data_d:begin															//read data form ram, send the ack signal
				local_rdata					<= ram_out_b;									//read data
				local_ack_n					<= 1'b0;									//assert ack
				localbus_state				<= wait_cs_d;								//goto waiting
			end
			wait_cs_d: begin															//wait the cs
				ram_rden_b					<= 1'b0;									//clean all signal
				ram_wren_b					<= 1'b0;									//clean all signal
				if (local_cs_n == 1'b1)	begin											//The cs is high
					local_ack_n				<= 1'b1;									//The ack is low
					localbus_state			<= idle_d;									//goto idle
				end
				else begin																//The cs is low
					localbus_state			<= wait_cs_d;								//waiting cs
				end
			end
			default:
					localbus_state			<= idle_d;									//goto idle
			endcase
    end

end
//	wire			bit_comp2	= (ram_out_a == 192'b0)? 1'b1:1'b0;								//judge KEY==0
//	wire			bit_comp1	= (check_key_q_reg2[223:96] == ram_out_a[191:64])? 1'b1:1'b0;	//judge `KEY==KEY
//	wire			bit_comp0	= ((Dleft_out_key[63:32] + Dleft_out_key[95:64])>(ram_out_a[63:32] + ram_out_a[31:0]))? 1'b1:1'b0;	//judge `time-time > 100000
//	wire	[2:0]	bit_result 	= {bit_comp2,bit_comp1,bit_comp0}; 								//result
//look up the ram
always @(posedge Clk or negedge Reset_N)
	if (~Reset_N) begin
		Dleft_out_key_wr					<= 1'b0;									//clean all signal
		Dleft_out_key						<= 224'b0;									//clean all signal
		check_key_q_reg0					<= 224'b0;									//clean all signal
		check_key_q_reg1					<= 224'b0;									//clean all signal
		check_key_q_reg2					<= 224'b0;									//clean all signal
		ram_addr_a							<= {(RAMAddWidth){1'b0}};					//clean all signal
		ram_data_a							<= 192'b0;									//clean all signal
		ram_rden_a							<= 1'b0;									//clean all signal
		ram_wren_a							<= 1'b0;									//clean all signal
		check_key_rd						<= 1'b0;									//clean all signal
		count								<= 3'd0;									//clean all signal
		lookup_state						<= idle_s;									//clean all signal
	end
	else begin
		case(lookup_state)
		idle_s: begin																	
			Dleft_out_key_wr					<= 1'b0;								//clean signal
			Dleft_out_key						<= 224'b0;								//clean signal
			ram_rden_a							<= 1'b0;								//clean signal
			ram_wren_a							<= 1'b0;								//clean all signal
			ram_data_a							<= 192'b0;								//clean all signal
			check_key_q_reg0					<= 224'b0;								//clear
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key			
			if (check_key_empty == 1'b0 && Dleft_in_key_alf == 1'b0) begin				//address is coming
				check_key_rd					<= 1'b1;								//read the address fifo
				lookup_state					<= pharse_1_s;							//read-data need 2 cycle from ram 
				count							<= 3'd1;								//counter, record the read cycle
			end
			else begin																	//no address, wait
				check_key_rd					<= 1'b0;								//clean signal
				lookup_state					<= idle_s;								//waiting
			end
		end
		pharse_1_s: begin
			ram_rden_a							<= 1'b1;								//read the ram
			ram_addr_a							<= check_key_q[RAMAddWidth-1:0];		//send the address
			ram_wren_a							<= 1'b0;								//clean all signal
			ram_data_a							<= 192'b0;								//clean all signal
			check_key_q_reg0					<= check_key_q;							//restore the key
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
			if (check_key_usedw	> 4'h2)	begin											//hash address isn't empty, read
				check_key_rd					<= 1'b1;								//read fifo
				lookup_state					<= pharse_2_s;							//turn to pharse_2_s		
				count							<= count + 3'd1;						//counter=2
			end
			else begin																	//no address, wait data
				check_key_rd					<= 1'b0;								//don't read fifo
				lookup_state					<= wait_1_s;							//counter=1			
			end
		end
		pharse_2_s: begin
			ram_rden_a							<= 1'b1;								//read the ram
			ram_addr_a							<= check_key_q[RAMAddWidth-1:0];		//send the address
			check_key_q_reg0					<= check_key_q;							//restore the key
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
			if (check_key_usedw	> 4'h2)	begin											//hash address isn't empty, read
				check_key_rd					<= 1'b1;								//read fifo
				lookup_state					<= pharse_3_s;							//turn to pharse_3_s		
				count							<= count + 3'd1;						//counter=3
			end
			else begin																	//no address, wait data
				check_key_rd					<= 1'b0;								//don't read fifo
				lookup_state					<= wait_2_s;							//counter=2			
			end
		end	
		pharse_3_s: begin
			ram_rden_a							<= 1'b1;								//read the ram
			ram_addr_a							<= check_key_q[RAMAddWidth-1:0];		//send the address
			check_key_q_reg0					<= check_key_q;							//restore the key
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
			if (check_key_usedw	> 4'h2)	begin											//hash address isn't empty, read
				check_key_rd					<= 1'b1;								//read fifo
				lookup_state					<= pharse_4_s;							//turn to pharse_4_s		
				count							<= count + 3'd1;						//counter=4
			end
			else begin																	//no address, wait data
				check_key_rd					<= 1'b0;								//don't read fifo
				lookup_state					<= read_s;								//counter=3			
			end
		end	
		wait_1_s: begin																	//ram have 2 cycle to send data, so wait 2 cycle 
			ram_rden_a							<= 1'b0;								//clean signal
			lookup_state						<= wait_2_s;							//waiting data
		end
		wait_2_s: begin
			ram_rden_a							<= 1'b0;								//clean signal
			lookup_state						<= read_s;								//waiting data
		end
		pharse_4_s: begin
			ram_rden_a							<= 1'b1;
			ram_addr_a							<= check_key_q[RAMAddWidth-1:0];		//send the address
			check_key_q_reg0					<= check_key_q;							//restore the key
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
//			casex(bit_result)
//			3'b1xx:begin
//				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the data form ram
//				ram_wren_a						<= 1'b1;								//wirtie the data form ram
//				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY			
//			end
//			3'b01x:begin
//				ram_data_a						<= {ram_out_a[191:64],ram_out_a[63:32]+1'b1,check_key_q_reg2[63:32]};	//freq +1
//				ram_wren_a						<= 1'b1;								//wirtie the data form ram
//				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY			
//			end
//			3'b001:begin
//				ram_wren_a						<= 1'b1;								//wirtie the data form ram
//				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the new data form ram
//				Dleft_out_key_wr				<= 1'b1;								//send the
//				Dleft_out_key					<= {ram_out_a, check_key_q_reg2[31:0]};	//send the old to next				
//			end
//			default:begin
//				ram_wren_a						<= 1'b0;								//clean all signal
//				ram_data_a						<= 192'b0;								//clean all signal				
//				Dleft_out_key					<= check_key_q_reg2;					//send the KEY to next
//				Dleft_out_key_wr				<= 1'b1;								//send the KEY				
//			end
//			endcase
			//read the data form ram, then send the data
			if (ram_out_a == 192'b0) begin												//the list is empty
				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the data form ram
				ram_wren_a						<= 1'b1;								//wirtie the data form ram
				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY
			end
			else if (check_key_q_reg2[223:96] == ram_out_a[191:64]) begin					//The same KEY, freq +1
				ram_data_a						<= {ram_out_a[191:64],ram_out_a[63:32]+1'b1,check_key_q_reg2[63:32]};	//freq +1
				ram_wren_a						<= 1'b1;								//wirtie the data form ram
				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY
			end
			else if ((Dleft_out_key[63:32] + Dleft_out_key[95:64])>(ram_out_a[63:32] + ram_out_a[31:0])) begin		// timestamp > 10000
				ram_wren_a						<= 1'b1;								//wirtie the data form ram
				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the new data form ram
				Dleft_out_key_wr				<= 1'b1;								//send the
				Dleft_out_key					<= {ram_out_a, check_key_q_reg2[31:0]};	//send the old to next
			end
			else begin
				ram_wren_a						<= 1'b0;								//clean all signal
				ram_data_a						<= 192'b0;								//clean all signal				
				Dleft_out_key					<= check_key_q_reg2;					//send the KEY to next
				Dleft_out_key_wr				<= 1'b1;								//send the KEY
			end			
			if (check_key_usedw	> 4'h2)begin											//hash address isn't empty, read
				check_key_rd					<= 1'b1;								//read fifo
				lookup_state					<= pharse_4_s;							//countue send
			end
			else begin																	//no address, wait data
				check_key_rd					<= 1'b0;								//don't read fifo
				count							<= count - 3'd1;						//decrease 1
				lookup_state					<= read_s;								//counter=3
			end
		end
		wait_1_s: begin																	//ram have 2 cycle to send data, so wait 2 cycle 
			check_key_q_reg0					<= 224'b0;								//clear
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
			ram_rden_a							<= 1'b0;								//clean signal
			lookup_state						<= wait_2_s;							//waiting data
		end
		wait_2_s: begin
			check_key_q_reg0					<= 224'b0;								//clear
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
			ram_rden_a							<= 1'b0;								//clean signal
			lookup_state						<= read_s;								//waiting data
		end		
		read_s: begin
			check_key_q_reg0					<= 224'b0;								//clear
			check_key_q_reg1					<= check_key_q_reg0;					//restore the key
			check_key_q_reg2					<= check_key_q_reg1;					//restore the key
			ram_rden_a							<= 1'b0;								//clean signal
//			casex(bit_result)
//			3'b1xx:begin
//				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the data form ram
//				ram_wren_a						<= 1'b1;								//wirtie the data form ram
//				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY			
//			end
//			3'b01x:begin
//				ram_data_a						<= {ram_out_a[191:64],ram_out_a[63:32]+1'b1,check_key_q_reg2[63:32]};	//freq +1
//				ram_wren_a						<= 1'b1;								//wirtie the data form ram
//				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY			
//			end
//			3'b001:begin
//				ram_wren_a						<= 1'b1;								//wirtie the data form ram
//				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the new data form ram
//				Dleft_out_key_wr				<= 1'b1;								//send the
//				Dleft_out_key					<= {ram_out_a, check_key_q_reg2[31:0]};	//send the old to next				
//			end
//			default:begin
//				ram_wren_a						<= 1'b0;								//clean all signal
//				ram_data_a						<= 192'b0;								//clean all signal				
//				Dleft_out_key					<= check_key_q_reg2;					//send the KEY to next
//				Dleft_out_key_wr				<= 1'b1;								//send the KEY				
//			end
//			endcase
			//read the data form ram, then send the data
			if (ram_out_a == 192'b0) begin												//the list is empty
				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the data form ram
				ram_wren_a						<= 1'b1;								//wirtie the data form ram
				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY
			end
			else if (check_key_q_reg2[223:96] == ram_out_a[191:64]) begin					//The same KEY, freq +1
				ram_data_a						<= {ram_out_a[191:64],ram_out_a[63:32]+1'b1,check_key_q_reg2[63:32]};	//freq +1
				ram_wren_a						<= 1'b1;								//wirtie the data form ram
				Dleft_out_key_wr				<= 1'b0;								//don't send the KEY
			end
			else if ((Dleft_out_key[63:32] + Dleft_out_key[95:64])>(ram_out_a[63:32] + ram_out_a[31:0])) begin		// timestamp > 10000
				ram_wren_a						<= 1'b1;								//wirtie the data form ram
				ram_data_a						<= check_key_q_reg2[223:32];			//wirtie the new data form ram
				Dleft_out_key_wr				<= 1'b1;								//send the
				Dleft_out_key					<= {ram_out_a, check_key_q_reg2[31:0]};	//send the old to next
			end
			else begin
				ram_wren_a						<= 1'b0;								//clean all signal
				ram_data_a						<= 192'b0;								//clean all signal				
				Dleft_out_key					<= check_key_q_reg2;					//send the KEY to next
				Dleft_out_key_wr				<= 1'b1;								//send the KEY
			end	
			if (count == 3'd1)	begin 													//all read is empty
				lookup_state					<= idle_s;								//go back
			end			
			else  begin
				count							<= count - 3'd1;						//send 
				lookup_state					<= read_s;								//countine
			end			
		end
		default: begin
			lookup_state						<= idle_s;								//go back
		end
		endcase
	end


//fifo save address, this fifo is show-head fifo
//lpm_fifo#(
//		.data_width				(224						),
//		.data_usedw				(4							),
//		.data_depth				(16							)
//	)scfifo_192_16_address(
//		.aclr					(~Reset_N					),
//		.sclr					(~Reset_N					),
//		.clock					(Clk						),
//		.data					(Dleft_in_key				),
//		.rdreq					(check_key_rd				),
//		.wrreq					(Dleft_in_key_wr			),
//		.empty					(check_key_empty			),
//		.full					(							),
//		.q						(check_key_q				),
//		.usedw					(check_key_usedw			)
//	);
//PKT_FIFO
	wire						pkt0_Reset;		
	wire						pkt0_wrclock;	
	wire	[223:0]				pkt0_RamData;	
	wire						pkt0_RamRdreq;	
	wire						pkt0_RamWrreq;	
	wire	[3:0]				pkt0_rdaddress;	
	wire	[3:0]				pkt0_wraddress;	
	wire	[223:0]				pkt0_Ram_q;	
	fifo_top
	#(		.ShowHead			(1							),	//show head model,1<->show head,0<->normal
			.SynMode			(1							),	//1<->SynMode,0<->AsynMode
			.DataWidth			(224						),	//This is data width
			.DataDepth			(16							),	//for ASYNC,DataDepth must be 2^n (n>=1). for SYNC,DataDepth is a positive number(>=1)
			.RAMAddWidth		(4							)	//RAM address width, RAMAddWidth= log2(DataDepth).
	)scfifo_224_16_pkt_fifo(
			.aclr				(~Reset_N					),	//Reset the all signal, active high
			.data				(Dleft_in_key				),	//The Inport of data 
			.rdclk				(Clk						),	//ASYNC ReadClk
			.rdreq				(check_key_rd				),	//active-high
			.wrclk				(Clk						),	//ASYNC WriteClk, SYNC use wrclk
			.wrreq				(Dleft_in_key_wr			),	//active-high
			.q					(check_key_q				),	//The Outport of data
			.rdempty			(check_key_empty			),	//active-high
			.wrfull				(							),	//active-high
			.wrusedw			(check_key_usedw			),	//RAM wrusedword
			.rdusedw			(							),	//RAM rdusedword			
			.Reset				(pkt0_Reset					),	//The signal of reset, active high
			.wrclock			(pkt0_wrclock				),	//ASYNC WriteClk, SYNC use wrclk
			.rdclock			(							),	//ASYNC ReadClk
			.RamData			(pkt0_RamData				),	//RAM input data
			.RamRdreq			(pkt0_RamRdreq				),	//RAM read request
			.RamWrreq			(pkt0_RamWrreq				),	//RAM write request
			.rdaddress			(pkt0_rdaddress				),	//RAM read address
			.wraddress			(pkt0_wraddress				),	//RAM write address
			.Ram_q				(pkt0_Ram_q					)	//RAM output data			
	);
//	syn_ram#(
//			.DataWidth			(224						),	//This is data width
//			.DataDepth			(16							),	//for ASYNC,DataDepth must be 2^n (n>=1). for SYNC,DataDepth is a positive number(>=1)
//			.RAMAddWidth		(4							)	//RAM address width, RAMAddWidth= log2(DataDepth).			
//	)ram_224_16_pkt0(
//			.aclr				(pkt0_Reset					),  //ASYNC WriteClk, SYNC use wrclk
//			.clock				(pkt0_wrclock				),  //ASYNC ReadClk
//			.data				(pkt0_RamData				),  //RAM input data
//			.rdaddress			(pkt0_rdaddress				),  //RAM read request
//			.rden				(pkt0_RamRdreq				),  //RAM write request
//			.wraddress			(pkt0_wraddress				),  //RAM read address
//			.wren				(pkt0_RamWrreq				),  //RAM write address
//			.q					(pkt0_Ram_q					)   //RAM output data
//	);
	blk_mem_gen_0	
//	#(
//					.DataWidth	(224						),	//This is data width	
//					.DataDepth	(16							),	//for ASYNC,DataDepth must be 2^n (n>=1). for SYNC,DataDepth is a positive number(>=1)
//					.RAMAddWidth(4							)	//RAM address width, RAMAddWidth= log2(DataDepth).			
//	)
	ram_224_16_pkt0 (
					.clka		(pkt0_wrclock				),	//ASYNC WriteClk, SYNC use wrclk
					.ena		(pkt0_RamWrreq				),	//RAM write address
					.wea		(pkt0_RamWrreq				),	//RAM write address
					.addra		(pkt0_wraddress				),	//RAM read address
					.dina		(pkt0_RamData				),	//RAM input data
					.douta		(							),
					.clkb		(pkt0_wrclock				),	//ASYNC WriteClk, SYNC use wrclk
					.enb		(pkt0_RamRdreq				),  //RAM write request
					.web		(1'b0						),
					.addrb		(pkt0_rdaddress				),  //RAM read request
					.dinb		(263'b0						),
					.doutb		(pkt0_Ram_q					)	//RAM output data				
				);	
//ram
//lpm_ram#(
//	.data_width					(192						),
//	.data_usedw					(RAMAddWidth				),
//	.data_depth					(DataDepth					)
//)
// hash_RAM(
//	.address_a					(ram_addr_a					),
//	.address_b					(ram_addr_b					),
//	.clock						(Clk						),
//	.data_a						(ram_data_a					),
//	.data_b						(ram_data_b					),
//	.rden_a						(ram_rden_a					),
//	.rden_b						(ram_rden_b					),
//	.wren_a						(ram_wren_a					),
//	.wren_b						(ram_wren_b					),
//	.q_a						(ram_out_a					),
//	.q_b						(ram_out_b					)
//);	
//--------------------hash 2 ram----------------------------//
	wire							hash2_clka;		
	wire							hash2_ena;	
	wire							hash2_wea;	
	wire	[RAMAddWidth-1:0]		hash2_addra;		
	wire	[191:0]					hash2_dina;		
	wire	[191:0]					hash2_douta;		
	wire							hash2_clkb;		
	wire							hash2_enb;	
	wire							hash2_web;	
	wire	[RAMAddWidth-1:0]		hash2_addrb;		
	wire	[191:0]					hash2_dinb;		
	wire	[191:0]					hash2_doutb;		

	ASYNCRAM#(
					.DataWidth	(192						),	//This is data width	
					.DataDepth	(DataDepth					),	//for ASYNC,DataDepth must be 2^n (n>=1). for SYNC,DataDepth is a positive number(>=1)
					.RAMAddWidth(RAMAddWidth				)	//RAM address width, RAMAddWidth= log2(DataDepth).			
	)	
	hash_2(
					.aclr		(~Reset_N					),	//Reset the all write signal	
					.address_a	(ram_addr_a					),	//RAM A port address
					.address_b	(ram_addr_b					),	//RAM B port assress
					.clock_a	(Clk						),	//Port A clock
					.clock_b	(Clk						),	//Port B clock	
					.data_a		(ram_data_a					),	//The Inport of data 
					.data_b		(ram_data_b					),	//The Inport of data 
					.rden_a		(ram_rden_a					),	//active-high, read signal
					.rden_b		(ram_rden_b					),	//active-high, read signal
					.wren_a		(ram_wren_a					),	//active-high, write signal
					.wren_b		(ram_wren_b					),	//active-high, write signal
					.q_a		(ram_out_a					),	//The Output of data
					.q_b		(ram_out_b					),	//The Output of data
					// ASIC RAM
					.reset		(							),	//Reset the RAM, active higt
					.clka		(hash2_clka					),	//Port A clock
					.ena		(hash2_ena					),	//Port A enable
					.wea		(hash2_wea					),	//Port A write
					.addra		(hash2_addra				),	//Port A address
					.dina		(hash2_dina					),	//Port A input data
					.douta		(hash2_douta				),	//Port A output data
					.clkb		(hash2_clkb					),	//Port B clock
					.enb		(hash2_enb					),	//Port B enable
					.web		(hash2_web					),	//Port B write
					.addrb		(hash2_addrb				),	//Port B address
					.dinb		(hash2_dinb					),	//Port B input data
					.doutb		(hash2_doutb				)	//Port B output data	
	);
	blk_mem_gen_1	
//	#(
//					.DataWidth	(192						),	//This is data width	
//					.DataDepth	(DataDepth					),	//for ASYNC,DataDepth must be 2^n (n>=1). for SYNC,DataDepth is a positive number(>=1)
//					.RAMAddWidth(RAMAddWidth				)	//RAM address width, RAMAddWidth= log2(DataDepth).			
//	)
	hash2_ram_1024_192(
					.clka		(hash2_clka					),	//ASYNC WriteClk, SYNC use wrclk
					.ena		(hash2_ena					),	//RAM write address
					.wea		(hash2_wea					),	//RAM write address
					.addra		(hash2_addra				),	//RAM read address
					.dina		(hash2_dina					),	//RAM input data
					.douta		(hash2_douta				),	//RAM output data
					.clkb		(hash2_clkb					),	//ASYNC WriteClk, SYNC use wrclk
					.enb		(hash2_enb					),  //RAM write request
					.web		(hash2_web					),	//RAM write address
					.addrb		(hash2_addrb				),  //RAM read request
					.dinb		(hash2_dinb					),	//RAM input data
					.doutb		(hash2_doutb				)	//RAM output data				
				);		
		
		
endmodule