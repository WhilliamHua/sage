

////////////////////////////////////////////////////////////
//(1)initialize CPU, LED, UART, CHP, SATA
//(2)include signature FIS sent
//(3)will be called whever PHY is down.
////////////////////////////////////////////////////////////
void init()
{
    u16 ii;
    //(1)DW8052 setting
	TS8052_CKCON 	= 0x11; 	 // program strecth = 1 ; set clock data stretch = 1;
	TS8052_IE =  0x85;           //enable ext_int0 and ext_int1
	TS8052_IPL = 0x04;           //set external interrupt 1 priority.
	disable_code_program;
    //TS8052_IT1  |= 1;                    //set it to falling edge trigger
   
    
    //(2)LED setting    
	//init LED0
	sata.led0_blink = 0x1f;
	sata.led0_cntl = 0x80;      //rom is using 0xd4  
	
	sata.power_cntl = (power_cntl_en_init | power_cntl_en_oob | power_cntl_en_auto_power);
	
	//init LED1, wait for customer request, and TK need to fill this.
    //sata.led1_tunit= 0x0f;
    //sata.led1_repeat= 0x10;
    //sata.led1_period= 0x10;
	//sata.led1_cntl=0x00;

    
    //(3)UART setting
    #ifdef ENABLE_PRINT
    sata.uart_config = 0x14;    //enable UART and RX_INT_EN
    sata.uart_status = sata.uart_status | 0x40; //set CHP interrupt enable: bit 6 in uart_status
    #endif
    sata.uart_rx_stat1 |= 0x80; //reset the rx fifo
    sata.uart_int = 0x80;   //disable parity
    sata.uart_baudrate_low = 0x1e;      //max baudrate for utility
    sata.uart_baudrate_high = 0x00;
    sata.uart_rx_thresh_low = 0x04;
    
    
	g_flag = 0x00;
    g_enable_write_cache = 1;
    //(4)CHP init
    SFR_CHP_en = DEF_CHP_EN;
    sata.CHP_stripe = DEF_CHP_STRIPE;
    sata.CHP_stripe_h = DEF_CHP_STRIPE_H;

	
	SFR_watch_dog_high = 0x00;
    if(sata.flag_0 == flag_0_default)//this is the hardware reset value
    {
        while((sata.CHP0_status & 0x0f) != 0x0A)
        {
        	//provide chance for chp interface to boot code successfully
        	if(SFR_watch_dog_high > 0x50)reset_chip;
        }
        //sata.flag_0 = flag_0_idle;
        delay_ms(1);
        sata.CHP_clock_set = DFT_CHP_CLOCK_SET;
        delay_ms(1);
    }
	stop_all_chp; //we stop three times to ensure wake up all chp
	delay(300);
	stop_all_chp;
	delay(300);
	stop_all_chp;
    chp_clock_on();
   
    
	
    sata.CHP_cmd = CHP_CMD_STOP;
    wait_chp_ready();
    
	sata.CHP_cmd = CHP_CMD_INIT;
    wait_chp_ready();
	if(sata.flag_0 == flag_0_default)//this is the hardware reset value
	{
	    g_power_mode=power_mode_act;
	}
	if(sata.flag_0 == flag_0_default)//this is the hardware reset value
	{
		#ifdef SUPPORT_SMART1
		updata_smart(smart_power_num_addr,0x01);
        updata_smart(smart_power_down_addr,0x01);
        if(check_sd_mode)smart_card_update();
        else smart_erase_handle();
		#endif
	}
	else
    {
        #ifdef SUPPORT_SMART1
        updata_smart(smart_phy_down_addr,0x01);
        if(g_smart_power_down_flag == 1) updata_smart(smart_power_down_addr,0x01);
        reset_engine();
        #endif
    }
    #ifdef SUPPORT_SECURITY
    g_security_flag = 0x00;
    //security_state_flag = 0x00;
    if(sata.flag_0 == flag_0_default)//this is the hardware reset value
    {
        security_mode = 0x00;
        security_initial();
    }
    #endif
    if(sata.flag_0 == flag_0_default) sata.flag_0 = flag_0_idle;
    
    //(5)SATA init
    reset_engine();
    sata.led0_blink = 0xf4;
    //set data pending to zero
	#ifdef BUF_CMD_EN
	    sata.flag_0 = flag_0_idle;
	    cmd_buffer_init();
	    sata.flag_0 = flag_0_buf_init;
	#endif	
	
	//device shall enter active mode when device is power-up with the power-up in standby feature not implemented or not enabled  
	sata.force_CHP = 0x01;              //this is to enable multiple transfer in SUP burst
	sata.ncq_cntl =  ncq_cntl_block_rdy;
	sata.auto_power_set = 0x10;         //TODO: set hardware respond PMNAK.
    sata.link_tune |= link_tune_cont_dis;
    sata.ope_a = max_head_per_cylnd;
    sata.ope_b = max_sec_per_head;
	//to fix lecroy OOB6-7 test.
	sata.COMRESET_HIGH = 0x045;
	sata.COMWAKE_HIGH = 0x15;
	sata.COMWAKE_LOW = 0x0a;
	
    #ifdef SATA_1P5G_ONLY
            sata.phy_set_6 = 0x85;  //force SATA tx_speed 
	#endif

    /////phy setting //////////
    sata.phy_set_0 = 0x05    ;  // tx skew [2:0]
    sata.phy_set_4 = 0x2D    ;  //tx  emp[6:4]    drv[3:0]
    //sata.phy_set_6 = 0x83;
    //sata.phy_set_7 = 0x2d;
	
    #ifdef SUPPORT_NCQ
       enable_code_program;
       g_tmp_addr = NCQ_TABLE_BASE;
       for(i=0;i<NCQ_MAX_TAG;i++)
       {
           write_data(g_tmp_addr,TAG_FREE);
           g_tmp_addr = g_tmp_addr + NCQ_LINE_LEN;
       }
       disable_code_program;
    #endif

    #ifdef READ_ONLY
    chp_burst(CHP_CMD_READ,0x01,block_base_log);
    SFR_ext_sram_addr = 0x0007;
    read_only_flag = SFR_ext_sram_data;
    if(read_only_flag != read_only_enable) read_only_flag = read_only_disable;
    SFR_ext_sram_cntl = 0x00;
    #endif


    #ifdef SUPPORT_SMART
		//default enable.
        g_smart_flag = 0x01;
		g_smart_handle_flag = 0x00;
    	g_smart_test_status = 0x00;
        g_smart_write_cnt = 0x00;
		g_smart_read_cnt = 0x00;
		g_smart_block_cnt_l = 0x00;
        g_smart_block_cnt_h = 0x00;
		write_sec_cnt_l = 0x0000;
        write_sec_cnt_h = 0x00;
		read_sec_cnt_l = 0x0000;
		read_sec_cnt_h = 0x00;
    #endif

    sata.max_lba[0] = MAX_LBA0;
    sata.max_lba[1] = MAX_LBA1;
    sata.max_lba[2] = MAX_LBA2;
    sata.max_lba[3] = MAX_LBA3;
    sata.max_lba[4] = MAX_LBA4;
    sata.max_lba[5] = MAX_LBA5;
	
	/////////////////////////////////////////////////////////////////////////
        
    //if((sata.power_cntl & power_cntl_en_oob)!=0x00)goto check_phy_rdy;
start_phy:
    //myprintf("\nStart OOB.");
    g_current_fis_num = SFR_fis_num;   
    //start oob engine
    sata.power_cntl = (power_cntl_en_init | power_cntl_en_oob | power_cntl_en_auto_power);
	
	SFR_watch_dog_high = 0x00;
    while((sata.int2_status & int2_status_phyrdy)==0x00)
	{
        //112ms * 0x20 = 3s.  If not detect phy ready in 3s, reset PHY
        if(SFR_watch_dog_high>0x20)
        {
           SFR_watch_dog_high = 0x00;
           sata.rst_cntl = sata.rst_cntl | 0x02; //bit 1. reset PHY, this bit is self-clear
        }
	}

    //while((sata.int2_status & int2_status_phyrdy)==0x00);
   // for(ii=0;ii<200;ii++)
   // {
   //     delay(655);
   //     if((sata.int2_status & int2_status_phyrdy)!=0x00) goto phy_is_rdy;
   // }
   // sata.power_cntl = 0x00;
   // goto start_phy;
    


	for(ii=0;ii< 30000;ii++) //delay some time before send FIS34 to make sure phy is real ready
    {
        if(sata.int2_status& int2_status_phyrdy_n) goto start_phy;
    }
	////////////////////////////////////////////////////////////////////////
	//set interrupt enable
    sata.led0_blink = 0xf0;	

    myprintf("\nphyup");
    sata.int2_en |= int2_en_phyrdy_n;
	
	sata.int1_status = 0;	//clear other status

	//TODO: if wake from slumber state, need to send FIS34???
	//send the first FIS34
	init_tx_fis();
	tx_fis_34(status_good, 0x01, int_no);
	delay(100);//wait for FIS34 send done

	//avoid goto regulator int when power up progress
	#ifdef VOL_DET_EN
	//voltage detect
		sata.REGULATOR_SETTING_l = sata.REGULATOR_SETTING_l & 0xf0;
	
	    if(LOW_VOL_DET_SET != 0x00)sata.REGULATOR_SETTING_h = sata.REGULATOR_SETTING_h | int1_en_power_low;
		else sata.REGULATOR_SETTING_h = sata.REGULATOR_SETTING_h & (~int1_en_power_low);
    	
	#endif
 
	sata.ncq_cntl = sata.ncq_cntl & (~ncq_cntl_block_rdy);
	SFR_watch_dog_high = 0x00;
	
    while(g_current_fis_num == SFR_fis_num );
    sata.link_tune &= (~link_tune_cont_dis);     //turn off link_cont after the first FIS34 out.
}


////////////////////////////////////////////////////////////
//Whenever received a new FIS, the fields wll be updated to the content 
//of the new FIS.
////////////////////////////////////////////////////////////
void init_tx_fis()
{
	//such sequence in oder to save code size.
	sata.FIS_estatus     	 	= 0x00;
	sata.FIS_feature     	 	= 0x00;	   
	sata.FIS_feature_ext	 	= 0x00;	 
	sata.FIS_device				= 0x00; 
	sata.FIS_pm					= 0x00;
	sata.FIS_seccnt				= 0x01;
	sata.FIS_seccnt_ext	 		= 0x00; 
	sata.FIS_lba_low			= 0x01;
	sata.FIS_lba_low_ext		= 0x00; 
	sata.FIS_lba_mid	   		= 0x00; 
	sata.FIS_lba_mid_ext      	= 0x00; 
	sata.FIS_lba_high	    	= 0x00; 
	sata.FIS_lba_high_ext      	= 0x00; 
}


///////////////////////////////////////////////////////////////
//Now only handle PHY down
///////////////////////////////////////////////////////////////
void int0_handler() interrupt 0
{
	//block all interrupt from 8051
	//TS8052_IE = 0x00;
	disable_code_program;
	sata.int2_en &= ~int2_en_phyrdy_n;
	
	#ifdef BUF_CMD_EN
	if(sata.flag_0==flag_0_buf_init)
	{
	    while(buf_not_empty )cmd_buffer_read();
	}
	#endif
	
	sata.flag_0 = flag_0_idle;
	reset_engine();
	sata.link_tune |= link_tune_cont_dis;
	sata.ncq_cntl =  ncq_cntl_block_rdy;
	sata.rst_cntl = 0x10;   //reset the cpu, reset bit willl be cleared automatically by hardware.
}

////////////////////////////////////////////////////////////////
//UART, CHP interrupt, LOW_Voltage_detect
//ext interrupt 1 has higher priority then ext int 0
////////////////////////////////////////////////////////////////
void int1_handler() interrupt 2
{

    u8 tmp_flag;
    tmp_flag = 0x00;
    if((TS8052_PCON  & PCON_PWM) == PCON_PWM)
    {
        tmp_flag = 0x01;
        disable_code_program;
    }
    
    #ifdef VOL_DET_EN
	   //voltage detect
       check_power_loss();
	#endif
	
	#ifdef ENABLE_UART_INT
    if((sata.ext_int1_status & ext_int1_uart) == ext_int1_uart)uart_int_handler();
	#endif
	
	#ifdef ENABLE_CHP_INT
    if((sata.ext_int1_status & ext_int1_chp) == ext_int1_chp)chp_int_handler();
	#endif

	
    
	if(tmp_flag==0x01)
	{
	    enable_code_program;
	}
	
	
   
}


void wait_chp_ready()
{
    while(1)
    {
        //related bits in SFR_CHP_rdy indicate CHP in ready state.
        if(chp_is_ready)break;

		//int0_hanlder use this routine
		#ifdef VOL_DET_EN
			//voltage detect
			check_power_loss();
		#endif
    }
}

void wait_chp_not_ready()
{
	while(1)
	{
		if(chp_not_ready)break;
		#ifdef VOL_DET_EN
			//voltage detect
			check_power_loss();
		#endif
	}

}
void tx_fis_34(u8 status, u8 error, u8 flag)
{
	  sata.FIS_error	 = error;
	  sata.FIS_status    = status;
	  if(flag!=0)  sata.FIS_pm |= 0x40;  // I bit = 1
	  else         sata.FIS_pm &= 0xbf;  // I bit = 0
	  sata.FIS_type	=  0x34;
}

void tx_fis_5f(u8 status, u8 error, u8 flag, u8 seccnt, u8 seccnt_ext,u8 estatus)
{
	sata.FIS_error = error;
	sata.FIS_status = status;
	sata.FIS_pm = flag;
	sata.FIS_seccnt =  seccnt;
	sata.FIS_seccnt_ext = seccnt_ext;		
	sata.FIS_pio_xfer_cnt_l = 0x00; //attention: asume this is zero
	//sata.FIS_pio_xfer_cnt_h = seccnt << 1;
	sata.FIS_pio_xfer_cnt_h = 0x02; //attention:force one sector
	sata.FIS_estatus = estatus;
	sata.FIS_type = 0x5f;           //trigger the transmission
}


/////////////////////////////////////////////////////////////////
//data flow between SUP 32K FIFO and SATA
/////////////////////////////////////////////////////////////////
void sata_burst(u8 dir, u8 cnt)
{
	SFR_sata_burst_base = 0x0000;
	SFR_sata_burst_cnt_l = cnt;
	SFR_sata_burst_cnt_h = 0x00;
	//we may recover from abnormal state: and running is set.
	clear_burst_run;
	//if(dir==DIR_READ)set_burst_run_rd;
	//else set_burst_run_wr;
	SFR_sata_burst_cntl = dir;
	while(!sata_burst_done);
	//clear_burst_run;	
}


void bist_handler()
{
    //BYTE jj;
    //change PHY setting here.
	sata.phy_set_4 = 0x2D; //for eye diagram
	
    sata.phy_set_6 = 0x83;
    sata.phy_set_7 = 0x2b;
    /*
    sata.int2_en = 0;
    for(jj=0;jj<200;jj++)
    {
	delay(65534);
	sata.flag_0 = sata.phy_set_8;
	}
    delay(65534);
    sata.power_cntl = 0x00;
    while(1)
    {
    //myprintf("cpt_out=%x",sata.flag_0);
    }
	if((SFR_FIS0_2 & 0x10)!=0){
		
		//sata.bist_mode = 0x01; //bist_l
		while((sata.int1_status & 0x80) != 0x80 );

		
		sata.bist_mode = 0x00;
	}
	else sata.bist_mode = 0x01;
	*/
}

void save_lba()
{
    enable_fetch_lba;
    g_lba[0] = sata.SUP_LBA0;
    g_lba[1] = sata.SUP_LBA1;
    g_lba[2] = sata.SUP_LBA2;
    g_lba[3] = sata.SUP_LBA3;
    g_lba[4] = sata.SUP_LBA4;
    g_lba[5] = sata.SUP_LBA5;
    disable_fetch_lba;
}
void recover_lba()
{
    sata.SUP_LBA0 = g_lba[0];
    sata.SUP_LBA1 = g_lba[1];
    sata.SUP_LBA2 = g_lba[2];
    sata.SUP_LBA3 = g_lba[3];
    sata.SUP_LBA4 = g_lba[4];
    sata.SUP_LBA5 = g_lba[5];    
}

void check_power_loss()
{
	//this is the POK bit of regulator, double check several times to filter glitch.
	if((sata.ext_int1_status & ext_int1_poweroff) != ext_int1_poweroff)return;
	if((sata.ext_int1_status & ext_int1_poweroff) != ext_int1_poweroff)return;
	if((sata.ext_int1_status & ext_int1_poweroff) != ext_int1_poweroff)return;
	if((sata.ext_int1_status & ext_int1_poweroff) != ext_int1_poweroff)return;
	//not too much delay, since we need to close CHP ASAP

	sata.led0_blink = 0x0f;	//close led to save some power
	
	//(1)reset CHP, close CHP clock.
	sata.CHP_clock_set = 0x80 | 0x40 | 0x00;	
	//(2)close PHY
	sata.phy_cntl |= (phy_cntl_pd_rx | phy_cntl_pd_tx);
	sata.phy_set_0 |= phy_set_pd_tx_pll;
	
	//(3)if PHY is not closed, reset the chip.
	delay_ms(2);
	reset_chip;
	
	
}


