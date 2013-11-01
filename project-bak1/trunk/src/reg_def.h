#ifndef REG_DEF_H
#define REG_DEF_H

typedef struct
{
	//BYTE	RESV000;			//00
	BYTE	flag_0;			//00
	BYTE	FIS_error;		
	BYTE	RESV002;     //02		
	BYTE	RESV003;	
	
	BYTE	FIS_estatus;		//04
	BYTE	FIS_pio_xfer_cnt_l;	
	BYTE	FIS_pio_xfer_cnt_h;	
	BYTE	RESV007;			
	
	BYTE	RESV008; 			//08
	BYTE	RESV009; 			
	BYTE	RESV00A; 			
	BYTE	FIS_status;		
	
   	DWORD	RESV00CDEF;			//0c
   	
	BYTE	FIS_feature;		//10
 	BYTE	FIS_feature_ext;  	
	BYTE	FIS_device;		
	BYTE	FIS_pm;			

	BYTE	FIS_seccnt;			//14
	BYTE	FIS_seccnt_ext;	
	BYTE	FIS_lba_low;		
	BYTE	FIS_lba_low_ext; 
	
	BYTE	FIS_lba_mid;		//18
	BYTE	FIS_lba_mid_ext;
	BYTE	FIS_lba_high;  	
	BYTE	FIS_lba_high_ext;	

	BYTE    RESV01C;            //1C
	BYTE    RESV01D;
	BYTE    ncq_cntl;
	BYTE    quick_sactive;

	BYTE	phy_cntl;			//20
	BYTE	RESV021;	
	BYTE 	RESV022;	
	BYTE	RESV023;	

	DWORD	RESV024;			//24
	
	DWORD	RESV028;			//28
	
	DWORD	RESV02C;			//2c
	
	DWORD	RESV030;			//30
	
	BYTE	rst_cntl;			//34
	BYTE	int1_status;	
	BYTE	int1_en;	
	BYTE	FIS_type;
	
	BYTE	bist_mode;	  		//38
	BYTE	RESV039;  
  	BYTE	RESV03A;	    
  	BYTE	RESV03B;   		
  	
  	BYTE	power_cntl;	   		//3c
  	BYTE	int2_status;	 	
  	BYTE	int2_en;			
	BYTE	RESV03F; 			

    BYTE    FIS_content_1_0;    //40
    BYTE    FIS_content_1_1;
    BYTE    FIS_content_1_2;
    BYTE    FIS_content_1_3;

    BYTE    FIS_content_2_0;    //44
    BYTE    FIS_content_2_1;
    BYTE    FIS_content_2_2;
    BYTE    FIS_content_2_3;

    BYTE    FIS_content_4_0;    //48
    BYTE    FIS_content_4_1;
    BYTE    FIS_content_4_2;
    BYTE    FIS_content_4_3;

    BYTE    FIS_content_5_0;    //4C
    BYTE    FIS_content_5_1;
    BYTE    FIS_content_5_2;
    BYTE    FIS_content_5_3;
	

	DWORD	RESV050;		    //50		

	BYTE	ope_a;       		//54
	BYTE	ope_b;       
	BYTE	auto_power_set;       
	BYTE	CHP_stripe_h;       
	
	BYTE	sactive_0;          //58
	BYTE	sactive_1;       
	BYTE	sactive_2;      
	BYTE	sactive_3;       
	
	BYTE	CHP0_status_high;   //5C
	BYTE	CHP1_status_high;			
	BYTE	CHP2_status_high;		
	BYTE	CHP3_status_high;	    

	BYTE      phy_set_0;       //60
	BYTE      phy_set_1;
	BYTE      phy_set_2;
	BYTE      phy_set_3;

    BYTE      phy_set_4;       //64
    BYTE      phy_set_5;
	BYTE      phy_set_6;
    BYTE      phy_set_7;
 
    BYTE      phy_set_8;        //68      
    BYTE      phy_set_9;
	WORD	RESV06A;            //6A
	
	
	DWORD	RESV06C;			//6c


	BYTE	CHP0_status;		//70
	BYTE	CHP1_status;			
	BYTE	CHP2_status;		
	BYTE	CHP3_status;		
	
	BYTE	CHP4_status;		//74
	BYTE	RESV076;		
	BYTE	CHP4_status_high;		
	BYTE	RESV077;		

	BYTE	SUP_LBA0;			//78
	BYTE	SUP_LBA1;		
	BYTE	SUP_LBA2;		
	BYTE	SUP_LBA3;		
	
	BYTE	SUP_LBA4;			//7c
	BYTE	SUP_LBA5;		
	BYTE	CHP_cmd;		
	BYTE	CHP_stripe;	

	BYTE 	fis_rcv_content[32];//80-9f

	 
    BYTE    COMRESET_LOW;       //a0
    BYTE    COMRESET_HIGH;
    BYTE    COMWAKE_LOW;
    BYTE    COMWAKE_HIGH;
     
    BYTE    RESV0A4;            //a4
    BYTE    RESV0A5;
    BYTE    RESV0A6;
    BYTE    RESV0A7;
	
	WORD	fifo_base_sata;     //a8
	WORD	fifo_base_chp;        
	
	BYTE	counter_low;		//ac
	BYTE	counter_mid;	
	BYTE	counter_high;	
	BYTE	force_CHP;		

    DWORD   RESV0B0;
    DWORD   RESV0B4;
 
	
	//DWORD	watch_dog_cnt;		//b8
    BYTE    watch_dog_cnt_high; //b8
    BYTE    watch_dog_cnt_mid;  //b9
    BYTE    watch_dog_cnt_low;  //ba
    BYTE    watch_dog_low;      //bb
	
	BYTE	watch_dog_cntl;		//bc
	BYTE	RESV0BD;  
	
	BYTE    REGULATOR_SETTING_l;  //be-bf
	BYTE    REGULATOR_SETTING_h;
	BYTE	max_lba[6];			//c0
	
	BYTE	chp_bus_sel;	    //c6
	BYTE	ext_int1_status;		
	
	BYTE	RESV0C8;			//c8
	BYTE	RESV0C9;		
	BYTE	RESV0CA;		
	BYTE	RESV0CB;	
	
	BYTE	CHP_clock_set;	    //cc
	BYTE	CHP_cmd_sel;		    
	BYTE	link_tune;		    
	BYTE	oob_tune;		

    BYTE    uart_config;        //d0
    BYTE    uart_int;           //d1
    BYTE    uart_status;        //d2
    BYTE    RESV0D3;            

    BYTE    uart_baudrate_low;  //d4
    BYTE    uart_baudrate_high; 
    BYTE    RESV0D6;
    BYTE    RESV0D7;
    
    BYTE    uart_rx_stat0;      //d8
    BYTE    uart_rx_stat1;            
    BYTE    uart_rx_stat2;       
    BYTE    uart_rx_stat3;

    BYTE    uart_tx_stat0;      //dc
    BYTE    uart_tx_stat1;            
    BYTE    uart_tx_stat2;       
    BYTE    uart_tx_stat3;

    BYTE    uart_tx_data;       //e0
    BYTE    uart_rx_data;       //e1
    BYTE    uart_rx_thresh_low; //e2
    BYTE    uart_rx_thresh_high;//e3
    
    BYTE    uart_tx_thresh_low; //e4
    BYTE    uart_tx_thresh_high;//e5
    BYTE    RESV0E6;
    BYTE    RESV0E7;

    BYTE    RESV0E8;            //e8
    BYTE    RESV0E9;
    BYTE    RESV0EA;
    BYTE    RESV0EB;
    
    BYTE    RESV0EC;            //ec
    BYTE    RESV0ED;
    BYTE    RESV0EE;
    BYTE    RESV0EF;

    BYTE	led0_blink;     	//f0
	BYTE	led0_cntl;	     
	BYTE	led1_tunit;   
	BYTE	led1_period;     			
	
	BYTE	led1_bright;    	//f4
	BYTE	led1_repeat;   
	BYTE	led1_cntl;	   
	BYTE	RESV0F7;  

}reg_type;

#ifndef JUSTFORSOURCEINSIGHT
reg_type volatile xdata  sata   _at_ 0xff00;
#else
reg_type volatile xdata sata;
#endif



//and the following is SFR for S681

sfr   	 	SFR_FIS0_0 		        = 0x91; 	
sfr   	 	SFR_FIS0_1 		        = 0x95; 	
sfr   	 	SFR_FIS0_2 		        = 0x96; 
sfr   	 	SFR_FIS0_3 		        = 0x97; 

sfr   	 	SFR_quick_cmd	        = 0x9e;  
sfr    	 	SFR_fis_num		        = 0x9f;	

sfr			SFR_spi_tag             = 0xA7;
sfr			SFR_spi_set	  	        = 0xAB;
sfr			SFR_spi_io	  	        = 0xAC;
sfr			SFR_spi_config	        = 0xAD;
sfr			SFR_spi_base	        = 0xAE;
sfr			SFR_spi_cmd	  	        = 0xAF;
sfr 		SFR_spi_bitmask	        = 0xB7;

sfr16 		SFR_smart_cnt	        = 0xBC;
sfr         SFR_smart_cnt_l         = 0xBC;
sfr         SFR_smart_cnt_h         = 0xBD;

sfr			SFR_lba_comp	        = 0xBE;

sfr			SFR_smart_load	        = 0xC9;		

sfr16 		SFR_SATA_done_num       = 0xCE;
sfr         SFR_SATA_done_num_l     = 0xCE;
sfr         SFR_SATA_done_num_m     = 0xCF;


sfr			SFR_cmd_aux             = 0xD1;	

sfr			SFR_CHP_rdy		        = 0xD6;
sfr			SFR_CHP_en	  	        = 0xD7;


sfr			SFR_watch_dog_high	    = 0xD9;// if 3G: 56ms    // if 1.5G 112 ms   

sfr16 		SFR_sata_burst_cnt	    = 0xDE; 
sfr	 		SFR_sata_burst_cnt_l    = 0xDE;
sfr	 		SFR_sata_burst_cnt_h    = 0xDF;

sfr         SFR_SATA_done_num_h     = 0xE5;

sfr16 		SFR_sata_burst_base	    = 0xE6;	
sfr	 		SFR_sata_burst_base_l   = 0xE6;
sfr	 		SFR_sata_burst_base_h   = 0xE7;

sfr	SFR_sata_burst_cntl   		    = 0xE8;
	sbit  sata_burst_done		= SFR_sata_burst_cntl ^ 0;	
	sbit  sata_burst_type		= SFR_sata_burst_cntl ^ 1;	
	sbit  sata_burst_abort		= SFR_sata_burst_cntl ^ 2;		
	sbit  sata_burst_DMAT		= SFR_sata_burst_cntl ^ 3;		
	sbit  sata_burst_app_dma	= SFR_sata_burst_cntl ^ 4;	
	sbit  sata_burst_cont		= SFR_sata_burst_cntl ^ 5;	
	sbit  sata_burst_dir		= SFR_sata_burst_cntl ^ 6;	
	sbit  sata_burst_run  		= SFR_sata_burst_cntl ^ 7;	
	
	

sfr16 	SFR_ext_sram_addr		    = 0xF1;	
sfr		SFR_ext_sram_addr_l    	    = 0xF1;
sfr		SFR_ext_sram_addr_h    	    = 0xF2;

sfr		 SFR_ext_sram_data    		= 0xF3;	
sfr		 SFR_ext_sram_cntl     		= 0xF4;	


sfr		 SFR_copy_cnt    		    = 0xF5;
sfr		 SFR_cpu_reboot     	    = 0xF6;

sfr		SFR_dma_cntl     		    = 0xF8;
	sbit  dma_done				= SFR_dma_cntl ^ 0;	
	sbit  dma_resv				= SFR_dma_cntl ^ 1;	
	sbit  dma_pre_read			= SFR_dma_cntl ^ 2;	
	sbit  dma_conj				= SFR_dma_cntl ^ 3;	
	sbit  dma_auto_act			= SFR_dma_cntl ^ 4;	
	sbit  dma_cont			= SFR_dma_cntl ^ 5;
	sbit  dma_dir		  		= SFR_dma_cntl ^ 6;
	sbit  dma_run 				= SFR_dma_cntl ^ 7;	


sfr16	 SFR_dma_base    		    = 0xF9;	
sfr			 SFR_dma_base_l     	= 0xF9;
sfr			 SFR_dma_base_h     	= 0xFA;
	
sfr16	 SFR_dma_max_num		    = 0xFB;	
sfr   		 SFR_dma_max_num_l		= 0xFB;
sfr   		 SFR_dma_max_num_h		= 0xFC;

sfr16	 SFR_dma_done_num		    = 0xFD;	
sfr   		 SFR_dma_done_num_l	    = 0xFD;
sfr   	 	 SFR_dma_done_num_m	    = 0xFE;
sfr   		 SFR_dma_done_num_h	    = 0xFF;






//////////////////////////////////////////////////////////////////////
//BIT MASK
//////////////////////////////////////////////////////////////////////

//external register bit mask definition
//phy_cntl
#define phy_cntl_pd_rx						0x40
#define phy_cntl_pd_tx						0x80

//phy_set_0
#define phy_set_pd_tx_pll					0x80

//ncq_cntl
#define ncq_cntl_tx_fis_abort               0x01
#define ncq_cntl_new_fis27                  0x02
#define ncq_cntl_block_active               0x04
#define ncq_cntl_free                       0x08
#define ncq_cntl_set_app_cnt_high           0x10
#define ncq_cntl_set_dma_cnt_high           0x20
#define ncq_cntl_block_rdy                  0x40
#define ncq_cntl_link_idle                  0x80

//auto command
#define auto_cmd0_en                        0x01
#define auto_cmd1_en                        0x02
#define auto_cmd2_en                        0x04
#define auto_cmd3_en                        0x08
#define auto_reset                          0x10
#define auto_write_run_status                      0x40
#define auto_read_run_status                       0x80
//quick_sactive
#define quick_sactive_clear                 0x80
#define quick_sactive_set                   0x40


//link_tune
#define link_tune_cont_dis 				    0x02

//power_cntl
#define	power_cntl_en_partial      		    0x01	
#define	power_cntl_en_slumber      		    0x02	
#define	power_cntl_en_wakeup	   		    0x04
#define	power_cntl_en_sleep        		    0x08		
#define	power_cntl_en_auto_power   		    0x10	
#define	power_cntl_en_oob   	   		    0x20	
#define	power_cntl_en_init         		    0x40		
#define	power_cntl_rst_oob         		    0x80	


//int2_status
#define	int2_status_phyrdy_n			    0x01	
#define	int2_status_phyrdy				    0x02	

//int2_en
#define	int2_en_phyrdy_n				    0x01	
#define	int2_en_phyrdy					    0x02

//int1_en
#define int1_en_power_low                   0x08
#define osc_clk_en                          0x20


//ext_int1_status
#define ext_int1_watchdog                   0x01
#define ext_int1_fis                        0x02
#define ext_int1_sata                       0x04
#define ext_int1_chp                        0x08
#define ext_int1_rvs                        0x10
#define ext_int1_sde                        0x20
#define ext_int1_poweroff                   0x40
#define ext_int1_uart                       0x80



//dma control
#define SFR_dma_cntl_done 	                0x01
#define SFR_dma_cntl_resv 	                0x02
#define SFR_dma_cntl_pre_read 	            0x04
#define SFR_dma_cntl_conj 	                0x08
#define SFR_dma_cntl_auto_act 	            0x10
#define SFR_dma_cntl_cont 	                0x20
#define SFR_dma_cntl_dir_read 	            0x40
#define SFR_dma_cntl_run 	                0x80

//sata burst control
#define SFR_sata_burst_cntl_done            0x01
#define SFR_sata_burst_cntl_type            0x02
#define SFR_sata_burst_cntl_abort           0x04
#define SFR_sata_burst_cntl_DMAT            0x08
#define SFR_sata_burst_cntl_dma_act         0x10
#define SFR_sata_burst_cntl_cont            0x20
#define SFR_sata_burst_cntl_dir_read        0x40
#define SFR_sata_burst_cntl_run             0x80




///////////////////////////////////////////////////////////////////
//SFr registers bit mask define:
///////////////////////////////////////////////////////////////////

//SFR_cmd_aux
#define	cmd_aux_srst		    0x01	
#define	cmd_aux_nIEN		    0x02	
#define	cmd_aux_hob			    0x04	
#define	cmd_aux_cbit		    0x08	
#define	cmd_aux_auto_reset	    0x10	
#define	cmd_aux_lba_mode	    0x20	
#define	cmd_aux_same_lba	    0x40	
#define	cmd_aux_same_cmd	    0x80	

//SFR_lba_comp
#define lba_up_exceed		    0x40	
#define lba_increase_one        0x08    
#define lba_decrease_one        0x80    


//SFR_smart_load 
#define smart_load_addr			0x01	
#define smart_load_chs			0x02	
#define smart_load_sata_cnt		0x04	
#define smart_load_chp_cnt		0x08	
#define smart_load_add_sata_cnt	0x10	
#define smart_load_add_chp_cnt	0x20	
#define smart_load_ope_add		0x40	
#define smart_load_ope_mul		0x80	


//SFR_quick_cmd
#define quick_cmd_read_dma		0x01
#define quick_cmd_read_mul		0x02
#define quick_cmd_read_sec		0x04
#define quick_cmd_read_ext		0x08
#define quick_cmd_write_dma		0x10
#define quick_cmd_write_mul		0x20
#define quick_cmd_write_sec		0x40
#define quick_cmd_write_ext		0x80


/////////////////////////////////////////////////////////////////
//sata.flag_0
#define flag_0_default 0x00
#define flag_0_idle 0xff
#define flag_0_buf_init 0x55
/////////////////////////////////////////////////////////////////
#endif
