#ifdef ENABLE_CHP_INT

u8 chp_status_low;
u8 chp_status_high;
void update_chp_status(u8 chp_sel)
{
    if(chp_sel==0x01)
    {
        chp_status_low = sata.CHP0_status;
        chp_status_high= sata.CHP0_status_high;  
    }
    if(chp_sel==0x02)
    {
        chp_status_low = sata.CHP1_status;
        chp_status_high= sata.CHP1_status_high;  
    }
    if(chp_sel==0x04)
    {
        chp_status_low = sata.CHP2_status;
        chp_status_high= sata.CHP2_status_high;   
    }
    if(chp_sel==0x08)
    {
        chp_status_low = sata.CHP3_status;
        chp_status_high= sata.CHP3_status_high;
    }
    if(chp_sel==0x10)
    {
        chp_status_low = sata.CHP4_status;
        chp_status_high= sata.CHP4_status_high;  
    }
    chp_status_low &= 0xf0; //only fetch the higher 4 bit
}


void chp_int_handler()
{
    u8 tmp_chp_en;
	u8 tmp_chp_stripe;
	u8 tmp_chp_stripe_h;
    u8 tmp_sel = 0x01;
    u8 first_print = 0x01;
    u8 pre_chp_status_low = 0x00;
    u8 tmp_chp_id = 0x00;
	tmp_chp_en = SFR_CHP_en;
	tmp_chp_stripe = sata.CHP_stripe;
	tmp_chp_stripe_h = sata.CHP_stripe_h;
int_loop:
    update_chp_status(tmp_sel);
    if(chp_status_low == CHP_STATUS_IDLE)goto next_loop;
    pre_chp_status_low = chp_status_low;
    switch(chp_status_low)
    {
        case CHP_STATUS_PRINT:
        {
            #ifdef ENABLE_PRINT 
            #ifdef ENABLE_PRINT_CHP
            if(first_print==0x01)
            {
             //   myprintf("\nCHP-%x:",tmp_chp_id);
                first_print = 0x00;
            }
            if(chp_status_high==CHP_STATUS_HIGH_PRINT_DONE)myprintf("\n");
            else myprintf("%c",chp_status_high);
            #endif  //#ifdef ENABLE_PRINT_CHP
            #endif //#ifdef ENABLE_PRINT
            break;
        }
        default: 
        {
            myprintf("Invalid CHP STATUS LOW!!!");
            break;
        } 
    }
    
    SFR_CHP_en = tmp_sel;
    sata.CHP_cmd = CHP_CMD_INT_DONE;
    while(1)
    {
        update_chp_status(tmp_sel);
        if(chp_status_low== CHP_STATUS_IDLE)break;
        #ifdef VOL_DET_EN
			//voltage detect
			check_power_loss();
		#endif
    }
    sata.CHP_cmd = CHP_CMD_INT_IDLE;
    //SFR_CHP_en = DEF_CHP_EN;

    //additional loop for CHP_STATUS_PRINT
    if(pre_chp_status_low == CHP_STATUS_PRINT)
    {
        if(chp_status_high==CHP_STATUS_HIGH_PRINT_DONE)goto next_loop;
        while(1)
        {
            update_chp_status(tmp_sel);
            if(chp_status_low!=CHP_STATUS_IDLE)goto int_loop;
            #ifdef VOL_DET_EN
				//voltage detect
				check_power_loss();
		    #endif
        }
    }
    
next_loop:
    
    if(tmp_sel==0x10)goto chp_int_over;
    tmp_sel = tmp_sel << 1;
    tmp_chp_id += 0x01;
    first_print = 0x01;
    goto int_loop;
    
chp_int_over:
    SFR_CHP_en = tmp_chp_en;
	sata.CHP_stripe = tmp_chp_stripe;
	sata.CHP_stripe_h = tmp_chp_stripe_h;
    return;
}
#endif


void chp_clock_off()
{    
	//smart data handle
	#ifdef SUPPORT_SMART1
	if(g_smart_read_handle == 1)	
	{
		updata_smart(smart_read_addr,g_smart_read_cnt);
		g_smart_read_cnt = 0x00;
		g_smart_read_handle = 0;
	}
	if(g_smart_write_handle == 1)	
	{
		updata_smart(smart_write_addr,g_smart_write_cnt);
		g_smart_write_cnt = 0x00;
		g_smart_write_handle = 0;
	}
    reset_engine();
	#endif
	
    #ifdef ENABLE_PRINT_CHP
    sata.uart_status = sata.uart_status&(~0x40); //set CHP interrupt disable: bit 6 in uart_status
    #endif
    //for continue command, CHP is waiting for the STOP command
    //we need to let it finish the data transmission to lower layer(SD/MMC)
    reset_engine();
    sata.led0_blink = 0x0f;
   // reset_auto_read;
   // reset_dma_engine();
    
    if(check_sd_mode)         //if chp0 connect to sd/mmc
    {
        //power saving: close the CHP clock
        sata.CHP_cmd = CHP_CMD_SHUTDOWN_CLOCK;
        //wait for command sync to CHP
        delay(10);
        //reset CPU and close clock
        sata.CHP_clock_set = 0x80 | 0x40 | 0x00; 
		//sata.CHP_clock_set = ((DFT_CHP_CLOCK_SET | 0x80 | 0x40) & 0xf0); 
    }
	else
	{
        sata.CHP_cmd = CHP_CMD_FLASH_SLEEP;
		wait_chp_not_ready();
		delay(100);
	}
	g_chp_active = 0;
    g_cont_en = 0;
    #ifdef ENABLE_PRINT_CHP
    sata.uart_status = sata.uart_status | 0x40; //set CHP interrupt enable: bit 6 in uart_status
    #endif

    
#ifdef ENABLE_SUP_CLK_OFF
	
    myprintf("\noff");
    sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_block_rdy;
	delay(5); //wait for block R_RDY to take effect
	if((sata.ncq_cntl & ncq_cntl_link_idle) == 0x00)goto chp_clock_off_over;
	if(g_current_fis_num != SFR_fis_num)goto chp_clock_off_over;
    sata.power_cntl |= power_cntl_en_sleep;
	delay(30);
	chp_clock_off_over:
	sata.ncq_cntl = sata.ncq_cntl & (~ncq_cntl_block_rdy);
#endif

    
}

void chp_clock_on()
{   
    #ifdef ENABLE_PRINT_CHP
    sata.uart_status = sata.uart_status&(~0x40); //set CHP interrupt disable: bit 6 in uart_status
    #endif
    if(~g_chp_active)
    {
        if(check_sd_mode)
        {
            sata.CHP_cmd = CHP_CMD_SHUTDOWN_CLOCK; //TODO: confirm with TK
            
			sata.CHP_clock_set = DFT_CHP_CLOCK_SET;
			delay(10);
        }
		stop_all_chp;
		delay(100);
		stop_all_chp;
		delay(100);
		stop_all_chp;
        wait_chp_ready();     
    }
    g_chp_active = 1;
    #ifdef ENABLE_PRINT_CHP
    sata.uart_status = sata.uart_status | 0x40; //set CHP interrupt enable: bit 6 in uart_status
    #endif
}

void chp_burst(u8 cmd,u8 cnt,u8 base_addr)//read cnt should < chp_number * stripe, or sync fifo bug will be trigged  
{
	reset_engine();
	save_lba();
	sata.SUP_LBA0 = SMART_BASE_ADDR0;
	sata.SUP_LBA1 = SMART_BASE_ADDR1;
	sata.SUP_LBA2 = SMART_BASE_ADDR2;
	sata.SUP_LBA3 = SMART_BASE_ADDR3;
	sata.SUP_LBA4 = SMART_BASE_ADDR4;
	sata.SUP_LBA5 = SMART_BASE_ADDR5;
	for(;base_addr>0;base_addr--)
	{
        auto_increase_lba;
	}
	sata.CHP_cmd = cmd;
	SFR_dma_base = 0x0000;
	SFR_dma_max_num_l = cnt;
	SFR_dma_max_num_h = 0x00;
	if(cmd == CHP_CMD_WRITE)
    {
        SFR_dma_cntl = SFR_dma_cntl_run;
    }
    else
    {
    	SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_dir_read;
    }
	while(!dma_done);
    clear_dma_run;
    recover_lba();
}

void flush_delay()
{
	reset_engine();
    #ifdef SUPPORT_SMART1
    if(g_smart_power_down_flag == 0)
    {
        updata_smart_n(smart_power_down_addr,0x01);
        g_smart_power_down_flag = 1;
        reset_engine();
    }
    #endif
	sata.CHP_cmd = CHP_CMD_FLUSH;
	wait_chp_ready();
	reset_engine();
}


