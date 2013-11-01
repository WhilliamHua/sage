

module makefile;

integer i,j,k;
reg[7:0] INIT_t;
integer fp;
reg [31:0] data [20000:0];

initial
begin
	for(i = 0; i < 512*2; i = i + 1)data[i]=32'hffffffff;
	$readmemh ("ROM.v",data);
	fp = $fopen("ROM_1KX32BIT.v");
end


initial
begin
	$fdisplay(fp,"`celldefine");
	$fdisplay(fp,"module ROM_1KX32BIT ( \n\t\tQ,\n\t\tCLK,\n\t\tCEN,\n\t\tA\n\t\t);");

	
	$fdisplay(fp,"output [31:0]            Q;");
	$fdisplay(fp,"input                    CLK;");
	$fdisplay(fp,"input                    CEN;");
	$fdisplay(fp,"input [9:0]             A;");

	$fdisplay(fp,"wire [31:0] DO [1:0];");
	$fdisplay(fp,"assign Q = DO[A[9]];");

	$fdisplay(fp,"	RAMB16 #");
	$fdisplay(fp,"	(");
	$fdisplay(fp,"		.SIM_COLLISION_CHECK(\"ALL\")," );
	$fdisplay(fp,"		.WRITE_MODE_A (\"WRITE_FIRST\"),");
	$fdisplay(fp,"		.WRITE_MODE_B (\"WRITE_FIRST\"),");
	$fdisplay(fp,"		.READ_WIDTH_A (36),");
	$fdisplay(fp,"		.READ_WIDTH_B (36),");
	$fdisplay(fp,"		.WRITE_WIDTH_A (36),");
	$fdisplay(fp,"		.WRITE_WIDTH_B (36)");
		
	$fdisplay(fp,"	)");
	$fdisplay(fp,"	u0_RAMB16");
	$fdisplay(fp,"	(");
	$fdisplay(fp,"	    .CASCADEOUTA(),");
	$fdisplay(fp,"	    .CASCADEOUTB(),");
	$fdisplay(fp,"	    .DOA(DO[0]),");
	$fdisplay(fp,"	    .DOB(),");
	$fdisplay(fp,"	    .DOPA(),");
	$fdisplay(fp,"	    .DOPB(),");
	$fdisplay(fp,"	    .ENA((A[9]==1'b0)?~CEN:1'b0),"); 
	$fdisplay(fp,"	    .CLKA(CLK), ");
	$fdisplay(fp,"	    .SSRA(1'b0), ");
	$fdisplay(fp,"	    .CASCADEINA(1'b0),"); 
	$fdisplay(fp,"	    .REGCEA(1'b0),");
	$fdisplay(fp,"	    .ENB(1'b0), ");
	$fdisplay(fp,"	    .CLKB(1'b0), ");
	$fdisplay(fp,"	    .SSRB(1'b0), ");
	$fdisplay(fp,"	    .CASCADEINB(1'b0),"); 
	$fdisplay(fp,"	    .REGCEB(1'b0),");
	$fdisplay(fp,"	    .ADDRA({1'b0,A[8:0],5'b00000}),");
	$fdisplay(fp,"	    .ADDRB(15'b111111111111111),");
	$fdisplay(fp,"	    .DIA(32'h00000000),");
	$fdisplay(fp,"	    .DIB(32'h00000000),");
	$fdisplay(fp,"	    .DIPA(4'h0),");
	$fdisplay(fp,"	    .DIPB(4'h0),");
	$fdisplay(fp,"	    .WEA(4'h0),");
	$fdisplay(fp,"	    .WEB(4'h0)");
	$fdisplay(fp,"	);\n\n");
	
	$fdisplay(fp,"	RAMB16 #");
	$fdisplay(fp,"	(");
	$fdisplay(fp,"		.SIM_COLLISION_CHECK(\"ALL\")," );
	$fdisplay(fp,"		.WRITE_MODE_A (\"WRITE_FIRST\"),");
	$fdisplay(fp,"		.WRITE_MODE_B (\"WRITE_FIRST\"),");
	$fdisplay(fp,"		.READ_WIDTH_A (36),");
	$fdisplay(fp,"		.READ_WIDTH_B (36),");
	$fdisplay(fp,"		.WRITE_WIDTH_A (36),");
	$fdisplay(fp,"		.WRITE_WIDTH_B (36)");
		
	$fdisplay(fp,"	)");
	$fdisplay(fp,"	u1_RAMB16");
	$fdisplay(fp,"	(");
	$fdisplay(fp,"	    .CASCADEOUTA(),");
	$fdisplay(fp,"	    .CASCADEOUTB(),");
	$fdisplay(fp,"	    .DOA(DO[1]),");
	$fdisplay(fp,"	    .DOB(),");
	$fdisplay(fp,"	    .DOPA(),");
	$fdisplay(fp,"	    .DOPB(),");
	$fdisplay(fp,"	    .ENA((A[9]==1'b1)?~CEN:1'b0),"); 
	$fdisplay(fp,"	    .CLKA(CLK), ");
	$fdisplay(fp,"	    .SSRA(1'b0), ");
	$fdisplay(fp,"	    .CASCADEINA(1'b0),"); 
	$fdisplay(fp,"	    .REGCEA(1'b0),");
	$fdisplay(fp,"	    .ENB(1'b0), ");
	$fdisplay(fp,"	    .CLKB(1'b0), ");
	$fdisplay(fp,"	    .SSRB(1'b0), ");
	$fdisplay(fp,"	    .CASCADEINB(1'b0),"); 
	$fdisplay(fp,"	    .REGCEB(1'b0),");
	$fdisplay(fp,"	    .ADDRA({1'b0,A[8:0],5'b00000}),");
	$fdisplay(fp,"	    .ADDRB(15'b111111111111111),");
	$fdisplay(fp,"	    .DIA(32'h00000000),");
	$fdisplay(fp,"	    .DIB(32'h00000000),");
	$fdisplay(fp,"	    .DIPA(4'h0),");
	$fdisplay(fp,"	    .DIPB(4'h0),");
	$fdisplay(fp,"	    .WEA(4'h0),");
	$fdisplay(fp,"	    .WEB(4'h0)");
	$fdisplay(fp,"	);\n\n");

	


$fdisplay(fp,"defparam ");


		for(j = 0;j<2 ; j=j+1)
		begin	
			for(INIT_t = 0; INIT_t < 64; INIT_t = INIT_t + 1)
			begin
				$fwrite(fp,"u%0d_RAMB16.INIT_",j);
				$fwrite(fp,"%h",INIT_t[7:4]);
				case(INIT_t[3:0])
					4'ha: $fwrite(fp,"A");
					4'hb: $fwrite(fp,"B");
					4'hc: $fwrite(fp,"C");
					4'hd: $fwrite(fp,"D");
					4'he: $fwrite(fp,"E");
					4'hf: $fwrite(fp,"F");
					default:$fwrite(fp,"%h",INIT_t[3:0]);
				endcase
				$fwrite(fp," = 256'h");

				for(k=0;k<8;k=k+1) $fwrite(fp,"%h",data[j*512 + INIT_t*8 + (7-k)]);
				if((j*512 + INIT_t*8 + k) == 512*2)$fwrite(fp,"; \n");
				else $fwrite(fp,", \n");
			end
		end
  
$fdisplay(fp,"endmodule  ");                           
$fdisplay(fp,"`endcelldefine");

end	

endmodule
