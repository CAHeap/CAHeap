// ****************************************************************************
// Copyright		: 	NUDT.
// ============================================================================
// FILE NAME		:	half32_64bit.v
// CREATE DATE		:	2016-5-5 
// AUTHOR			:	xiongzhiting
// AUTHOR'S EMAIL	:	
// AUTHOR'S TEL		:	
// ============================================================================
// RELEASE 	HISTORY		-------------------------------------------------------
// VERSION 			DATE				AUTHOR				DESCRIPTION
// 0.0	   		2016-5-5			xiongzhiting			Original Verison
// ============================================================================
// KEYWORDS 		: 	xor Hash algorithm
// ----------------------------------------------------------------------------
// PURPOSE 			: 	hash algorithm
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
module half32_128bit(
input	wire			clk,
input	wire			reset_n,
//-----------input key-------------//    
input	wire	[191:0]	data,
input	wire			datavalid,
input	wire			endofpacket,
//-----------output hash value----//
output	reg		[223:0]	checksum,
output	reg				crcvalid
);
    
//----------------------ram--------------------------//
	wire	[31:0]	key_value;
	assign	key_value = datavalid ?  data[191:160]^data[159:128]^data[127:96]^data[95:64] : 32'b0;
/************************************code**********************************/ 
always @ (posedge clk or negedge reset_n)
begin
    if (~reset_n)
    begin
        checksum	<= 224'hffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
        crcvalid	<= 1'b0;
    end
    else if(datavalid)
    begin//1 cycle, get the value of data
		checksum	<=	{data,key_value};	
		crcvalid	<=	1'b1;
    end
    else//data transmit is over
    begin
        checksum	<= 224'hffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
        crcvalid	<= 1'b0;
    end
end

endmodule
