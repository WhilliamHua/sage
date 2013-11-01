
/////////////////////////////////////////////////////////////
//(1)load LBA
//(2)check LBA error: LBA exceed, or CHS parameter...
/////////////////////////////////////////////////////////////

BYTE gen_check_sum()
{
    BYTE temp, sum;
    WORD jj;
        SFR_ext_sram_cntl = 0x10;   
        SFR_ext_sram_addr = 0x0000;
        sum= 0x00;
        for (jj= 0x00; jj< 511; jj++)   
        {
            temp = SFR_ext_sram_data;
            sum +=temp;
        }
        
        temp= ~sum + 1;
        return temp;
    }
    

void reset_engine()
{
	
    clear_dma_run; //this will also clear sata burst run
    SFR_CHP_en= DEF_CHP_EN;
    stop_all_chp;
    wait_chp_ready(); 
    g_cont_en = 0;
}

void reset_engine_read_write()
{
	{
		clear_dma_run; //this will also clear sata burst run
		SFR_CHP_en= DEF_CHP_EN;
		stop_all_chp;
		while(1)
		{
			 //related bits in SFR_CHP_rdy indicate CHP in ready state.
			 if(chp_is_ready)break;
			 if(g_current_fis_num != SFR_fis_num)break;
			 //int0_hanlder use this routine
         #ifdef VOL_DET_EN
			 //voltage detect
			 check_power_loss();
         #endif
		}
		g_cont_en = 0;
	}
}

void check_lba()
{
	g_flag_lba_err = 0; 
	//(1)LBA mode
	if(SFR_cmd_aux & cmd_aux_lba_mode) // if LBA mode
	{
	 
	if(SFR_lba_comp & lba_up_exceed) 
		{
			  myprintf("\nLBAext \n");
			  
			  enable_fetch_lba;
			  myprintf("\nLBAc0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
			  disable_fetch_lba;
			  
			  //g_flag_lba_err = 1;
       	}
	
	SFR_smart_load = smart_load_addr;
 	}
	//(2)CHS mode
	else
	{
	    //device not check lba, if host send error lba, that is a system error
		/*
		u16_t temp16; //this is cylinder
		temp16.byte.l = sata.FIS_lba_mid;
		temp16.byte.h = sata.FIS_lba_high;
		if(temp16.word > (max_cylnd-1))g_flag_lba_err = 1;			//invalid cylinder
		if((sata.FIS_device & 0x0f) > (max_head_per_cylnd -1))g_flag_lba_err = 1;	//invalid head
		if(sata.FIS_lba_low > max_sec_per_head)g_flag_lba_err = 1; //invalid sector
		if(sata.FIS_lba_low == 0)g_flag_lba_err = 1;
        */     
		//load the address to CHP, hardware will translate the CHS to lba for us:
		//myprintf("\nCHS \n");
		//SFR_smart_load = smart_load_addr| smart_load_chs;
		 if(security_mode==show_master)
		{
		 g_flag_lba_err = 1;	//in CHS mode, do not support address offset,(master's capacity must big than 8GB)
		 myprintf("\nMaster CHS");
		}
		else
		{
		 	SFR_smart_load = smart_load_addr| smart_load_chs;	
		}
	
	}
}

void security_check_lba()
{
	g_flag_lba_err = 0;
	//(1)LBA mode
	if(SFR_cmd_aux & cmd_aux_lba_mode) // if LBA mode
	{
	
	/*#ifdef SUPPORT_SECURITY1
	if(security_mode==show_user)
		{
		if(SFR_lba_comp & lba_up_exceed) 
			{
			  myprintf("\nLBAext \n");
			  g_flag_lba_err = 1;
			  tx_fis_34(status_bad,error_abort,int_set);
		         reset_engine();
		}
	}
	else if(security_mode==show_master)
		{
		if(SFR_lba_comp & lba_low_small) 
			{
			  myprintf("\nLBAext_low \n");
			  g_flag_lba_err = 1;
			  tx_fis_34(status_bad,error_abort,int_set);
		         reset_engine();
		}
	}
	#endif*/

	if(SFR_lba_comp & lba_up_exceed) 
			{
			  myprintf("\nSLBAext \n");
			  
			  enable_fetch_lba;
			  myprintf("\nSLBAc0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
			  disable_fetch_lba;
			  
			  g_flag_lba_err = 1;
			  tx_fis_34(status_bad,error_abort,int_set);
		         reset_engine();
		}
	
	SFR_smart_load = smart_load_addr;
	
#ifdef security_debug
	enable_fetch_lba;
	myprintf("\nLBAc0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;
#endif

	#ifdef SUPPORT_SECURITY1
       choose_lba();
        #endif

#ifdef security_debug		
	enable_fetch_lba;
	myprintf("\nLBAc1: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
#endif	
	
        
	 //myprintf("\nLBAc1 \n");
	}
	//(2)CHS mode
	else
	{
	    //device not check lba, if host send error lba, that is a system error
		/*
		u16_t temp16; //this is cylinder
		temp16.byte.l = sata.FIS_lba_mid;
		temp16.byte.h = sata.FIS_lba_high;
		if(temp16.word > (max_cylnd-1))g_flag_lba_err = 1;			//invalid cylinder
		if((sata.FIS_device & 0x0f) > (max_head_per_cylnd -1))g_flag_lba_err = 1;	//invalid head
		if(sata.FIS_lba_low > max_sec_per_head)g_flag_lba_err = 1; //invalid sector
		if(sata.FIS_lba_low == 0)g_flag_lba_err = 1;
        */     
		//load the address to CHP, hardware will translate the CHS to lba for us:
		//myprintf("\nCHS \n");
		//SFR_smart_load = smart_load_addr| smart_load_chs;
		if(security_mode==show_master)
		{
		 g_flag_lba_err = 1;	//in CHS mode, do not support address offset,(master's capacity must big than 8GB)
		}
		else
		 SFR_smart_load = smart_load_addr| smart_load_chs;	
	
	}
}


///////////////////////////////////////////////////////////////////
//add offset to chs, when in CHS mode
///////////////////////////////////////////////////////////////////
void security_check_chs()
{
	if(security_mode==show_master)
	{
	 g_flag_lba_err = 1;	//in CHS mode, do not support address offset,(master's capacity must big than 8GB)
	}
	else
	 SFR_smart_load = smart_load_addr| smart_load_chs;
		
		
	#ifdef security_debug_chs		
	enable_fetch_lba;
	myprintf("\nCHSc0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	#endif		

}



///////////////////////////////////////////////////////////////////
//also load sector count to g_sec_cnt
///////////////////////////////////////////////////////////////////
void check_sec_cnt()
{
    
	g_sec_cnt.word = SFR_smart_cnt;
}

//////////////////////////////////////////////////
//g_lba_add = sata.SUP_LBA + g_sec_cnt_l.
//only 4 byte LBA valid
//////////////////////////////////////////////////
void soft_lba_add()
{
	
	u8 tmp_data; 

		enable_fetch_lba;
		g_lba_add[0] = sata.SUP_LBA0;
		g_lba_add[1] = sata.SUP_LBA1;
		g_lba_add[2] = sata.SUP_LBA2;
		g_lba_add[3] = sata.SUP_LBA3;
		disable_fetch_lba;
		//(1)byte 0
		tmp_data = g_lba_add[0]; //LBA0
		if(g_sec_cnt.byte.l != 0x00)
		g_lba_add[0] += g_sec_cnt.byte.l;
		//(2)byte  1
		if((tmp_data > g_lba_add[0])||(g_sec_cnt.byte.l == 0x00))
		{
			tmp_data      = g_lba_add[1];
			g_lba_add[1] += 0x01;
			//(3)byte 2
			if(tmp_data > g_lba_add[1])
			{
				tmp_data      = g_lba_add[2];
				g_lba_add[2] += 0x01;
				//(4)byte 3
				if(tmp_data > g_lba_add[2])
				{
					tmp_data      = g_lba_add[3];
					g_lba_add[3] += 0x01;
				}
			}
		}

}

/////////////////////////////////////////////////////////////
//check sata.SUP_LBA0 equal g_lba_add
//4 byte valid.
//return 0: not equal
//return 1: equal
/////////////////////////////////////////////////////////////
u8 chs_cmp_lba()
{  
    enable_fetch_lba;
    if(sata.SUP_LBA0 != g_lba_add[0]) goto discontinue;
    if(sata.SUP_LBA1 != g_lba_add[1]) goto discontinue;
    if(sata.SUP_LBA2 != g_lba_add[2]) goto discontinue;
    if(sata.SUP_LBA3 != g_lba_add[3]) goto discontinue;
    //if(sata.SUP_LBA4 != g_lba_add[4]) goto discontinue;
    //if(sata.SUP_LBA5 != g_lba_add[5]) goto discontinue;
    disable_fetch_lba;
    return 0x01;
	
    discontinue:
    disable_fetch_lba;
    return 0x00;
}


void chs_read_handle()
{
    u8 temp_sel,temp8;
    u8 temp_lba_low;
	u8 boundary_check;
	
	//reset_engine();	
	security_check_chs();
	reset_engine_read_write();	//reset_engine and check srst in wait rdy	
	if(g_current_fis_num != SFR_fis_num)return;
	
	sata.CHP_cmd = CHP_CMD_READ;
	g_cont_en = 1;
	temp_sel = 0x00;
	if(STRIPE_8K_SET)boundary_check = 0x0f;
	else if(STRIPE_4K_SET)boundary_check = 0x07;
	else boundary_check = 0x03;
	
	SFR_smart_load = smart_load_chp_cnt | smart_load_sata_cnt;
	SFR_dma_base = 0x0000;
	SFR_dma_cntl = 0xd8; //we will not enable preread
	
	//#ifdef security_debug_chs		
	enable_fetch_lba;
	myprintf("\nCHSr0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	//#endif
	
chs_read_cont:
    if(SFR_CHP_en == 0x01)
    {
        temp_sel = 0x00;
        temp_lba_low = sata.SUP_LBA0;
        while(!sata_burst_done)
        {
        	temp8 = SFR_dma_done_num_l;
            if((((temp_lba_low + temp8) & boundary_check) == 0x00)&& (temp_sel != temp8))//4k  boundary
            {
                temp_sel = temp8;
                sata.CHP_cmd = CHP_CMD_NEXT_BURST;
            }
            if(g_current_fis_num != SFR_fis_num)return;
        }
    }
    else //not 1 channel
    {
	    while(!sata_burst_done)
	    {
	        if(temp_sel != sata.chp_bus_sel)
		    {
			    sata.CHP_cmd_sel = temp_sel;
			    sata.CHP_cmd = CHP_CMD_NEXT_BURST;
			    temp_sel = sata.chp_bus_sel;
			    sata.CHP_cmd_sel = 0xff;
     	    }
		    if(g_current_fis_num != SFR_fis_num)return;
		}
	}

	if(sata_burst_abort)goto chs_read_err;	
	
	tx_fis_34(status_good,error_no, int_set);
	if(((sata.CHP0_status_high & 0x80) == 0x80)||((sata.CHP1_status_high & 0x80) == 0x80)||((sata.CHP2_status_high & 0x80) == 0x80)||((sata.CHP3_status_high & 0x80) == 0x80)||((sata.CHP4_status_high & 0x80) == 0x80)) 
    {
       goto chs_read_over;
    }
	
	clear_dma_run;  //not preread
	
	
    soft_lba_add(); //this will update g_lba_add
	
	#ifdef security_debug_chs		
	enable_fetch_lba;
	myprintf("\nCHSr1: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	#endif	
	
	//check whether there is another continue command
	SFR_watch_dog_high = 0x00;
	while(1)
    {
        if(g_current_fis_num != SFR_fis_num)
        {
        //break;
            //normal_cmd_cont will only be updated when get a new command in RTL.
            //if it's a FIS27 - Control: normal_cmd_cont will stay in 1
            check_sec_cnt();
            //check_lba();
		   if((SFR_FIS0_2 == ata_read_dma) && check_chs_cmd)
		   {
		        //if not continue
			    if(chs_cmp_lba()==0x00)break;
			    
               // SFR_smart_load = smart_load_add_chp_cnt | smart_load_add_sata_cnt;
				g_current_fis_num = SFR_fis_num;  //we	may  not  go  to  main
			    sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
				//sata_burst_cont = 1;
               // dma_cont = 1;
                //to fix copy machine
                delay(1);
                //////////////////
               	SFR_smart_load = smart_load_chp_cnt | smart_load_sata_cnt;
	            SFR_dma_base = 0x0000;
	            SFR_dma_cntl = 0xd8; 
                goto chs_read_cont;
            }
            else  break;
        }
        if(SFR_watch_dog_high > 0x20) break;
    }
	goto chs_read_over;

chs_read_err:
    tx_fis_34(status_bad,error_abort,int_set);
	#ifdef SUPPORT_SMART1
    updata_smart(smart_crc_num_addr,0x01);
    #endif
chs_read_over:
	reset_engine();
	return;
}


void chs_write_handle()
{
	//reset_engine();
	security_check_chs();
	reset_engine_read_write();	//reset_engine and check srst in wait rdy	
	if(g_current_fis_num != SFR_fis_num)return;

	enable_fetch_lba;
	myprintf("\nCHSw0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
    
    #ifdef READ_ONLY  
		#ifdef BUF_CMD_EN
			buf_check_cnt = 0x00;
		#endif
    if(read_only_flag == read_only_enable) 
    {
        SFR_smart_load = smart_load_sata_cnt;
        set_burst_run_wr;
    }
    else 
	#endif
    {
        sata.CHP_cmd = CHP_CMD_WRITE;
    	g_cont_en = 1;
    	SFR_smart_load = smart_load_chp_cnt | smart_load_sata_cnt;
    	SFR_dma_base = 0x0000;
    	set_dma_run_wr;
	}
	
	//#ifdef security_debug_chs		
	enable_fetch_lba;
	myprintf("\nCHSw0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	//#endif	
	
chs_write_cont:
	while(!sata_burst_done)
		{
		if(g_current_fis_num != SFR_fis_num)return;
		}
	if(sata_burst_abort)goto chs_write_err;	
    
    #ifdef READ_ONLY
    if(read_only_flag == read_only_enable)
	{
		tx_fis_34(status_good,error_no, int_set);
		goto chs_write_over;
	} 
    #endif
    
	while(!dma_done)
		{
		if(g_current_fis_num != SFR_fis_num)return;
		}

	tx_fis_34(status_good,error_no, int_set);
	if(((sata.CHP0_status_high & 0x80) == 0x80)||((sata.CHP1_status_high & 0x80) == 0x80)||((sata.CHP2_status_high & 0x80) == 0x80)||((sata.CHP3_status_high & 0x80) == 0x80)||((sata.CHP4_status_high & 0x80) == 0x80)) 
    {
       goto chs_write_over;
    }
	clear_dma_run;

	
	soft_lba_add();
	
	#ifdef security_debug_chs		
	enable_fetch_lba;
	myprintf("\nCHSw0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	#endif	
	
	//check whether there is another continue command
	SFR_watch_dog_high = 0x00;
	while(1)
    {
        if(g_current_fis_num != SFR_fis_num)
        {	
        //break;
            check_sec_cnt();
           // check_lba();
            if((SFR_FIS0_2 == ata_write_dma) && check_chs_cmd)
			{
			    //if not continue
			    if(chs_cmp_lba()==0x00)break;
			    
				g_current_fis_num = SFR_fis_num;  
			    sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
                SFR_smart_load = smart_load_chp_cnt | smart_load_sata_cnt;
	            SFR_dma_base = 0x0000;
	            set_dma_run_wr;
                goto chs_write_cont;
            }
            else  break;
        }
        if(sata.watch_dog_cnt_low > 0x04) break;
    }
	goto chs_write_over;

chs_write_err:
    tx_fis_34(status_bad,error_abort,int_set);
	#ifdef SUPPORT_SMART1
    updata_smart(smart_crc_num_addr,0x01);
    #endif
chs_write_over:
	reset_engine();
	return;
}

void read_handler()
{
    u8 temp_sel, temp8;
    u8 temp_lba_low;
	u8 boundary_check;

#ifdef security_debug
	enable_fetch_lba;
	myprintf("\nLBAr0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;
#endif

    //the first coming in will always be considered as not continue
    //if not continue
    {
        security_check_lba();
        temp_lba_low = sata.SUP_LBA0;
	 reset_engine_read_write();	//reset_engine and check srst in wait rdy	
	 if(g_current_fis_num != SFR_fis_num)return;
		
#ifdef security_debug
	enable_fetch_lba;
	myprintf("\nLBAr1: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;
#endif

        sata.CHP_cmd = CHP_CMD_READ;
        SFR_smart_load = smart_load_chp_cnt | smart_load_sata_cnt;
        SFR_dma_base = 0x0000;
        SFR_dma_cntl = 0xdc; //we will enable preread
        g_cont_en = 1;
        temp_sel = 0x00;
		if(STRIPE_8K_SET)boundary_check = 0x0f;
		else if(STRIPE_4K_SET)boundary_check = 0x07;
		else boundary_check = 0x03;
    }
read_cont:
    if(SFR_CHP_en == 0x01)
    {
        while(!sata_burst_done)
        {
            temp8 = SFR_dma_done_num_l;
            if((((temp_lba_low + temp8) & boundary_check) == 0x00)&& (temp_sel != temp8))//4k  boundary
            {
                temp_sel = temp8;
                sata.CHP_cmd = CHP_CMD_NEXT_BURST;
            }
            if(g_current_fis_num != SFR_fis_num)return;
        }
    }
    else //not 1 channel
    {
        while(!sata_burst_done)
        {
     	    if(temp_sel != sata.chp_bus_sel)
		    {
			    sata.CHP_cmd_sel = temp_sel;
			    sata.CHP_cmd = CHP_CMD_NEXT_BURST;
			    temp_sel = sata.chp_bus_sel;
			    sata.CHP_cmd_sel = 0xff;
     	    }
		    if(g_current_fis_num != SFR_fis_num)return;
        }
    }
  
    if(sata_burst_abort)goto read_handler_err;
    tx_fis_34(status_good,error_no, int_set);
    if(((sata.CHP0_status_high & 0x80) == 0x80)||((sata.CHP1_status_high & 0x80) == 0x80)||((sata.CHP2_status_high & 0x80) == 0x80)||((sata.CHP3_status_high & 0x80) == 0x80)||((sata.CHP4_status_high & 0x80) == 0x80)) 
    {
       goto read_handler_over;
    }
    //check whether there is continue command.
	SFR_watch_dog_high = 0x00;
    while(1)
     {
     	if(SFR_CHP_en != 0x01)
     	{
     		if(temp_sel != sata.chp_bus_sel)
			{
				sata.CHP_cmd_sel = temp_sel;
				sata.CHP_cmd = CHP_CMD_NEXT_BURST;
				temp_sel = sata.chp_bus_sel;
				sata.CHP_cmd_sel = 0xff;
     		}
     	}
		else
		{
			temp8 =  SFR_dma_done_num_l;
            if((((temp_lba_low + temp8) & 0x07) == 0x00)&& (temp_sel != temp8))//4k  boundary
            {
                temp_sel = temp8;
                sata.CHP_cmd = CHP_CMD_NEXT_BURST;
            }
		
		}
        if(g_current_fis_num != SFR_fis_num)
        {
            //break;
            //normal_cmd_cont will only be updated when get a new command in RTL.
            //if it's a FIS27 - Control: normal_cmd_cont will stay in 1
            if(normal_cmd_cont && g_cont_en && ((SFR_quick_cmd & quick_cmd_read_dma) != 0x00) )
            {
                //to fix copy machine
                delay(1);
                //////////////////
                SFR_smart_load = smart_load_add_chp_cnt | smart_load_add_sata_cnt;
                sata_burst_cont = 1;
                //dma_cont = 1; //we have set pre-read, so this can be commented
                g_current_fis_num = SFR_fis_num;  
                sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
                goto read_cont;
            }
            else break;
        }
        if(SFR_watch_dog_high > 0x20) break;
    }
    #ifdef SUPPORT_SMART1
	smart_read_cnt();
	#endif
	goto read_handler_over;	
	
read_handler_err:
    
    tx_fis_34(status_bad,error_abort,int_set);
	#ifdef SUPPORT_SMART1
    updata_smart(smart_crc_num_addr,0x01);
    #endif
	//I am not sure host will retry that command immediatelly, need to break the continue mode
read_handler_over:
	reset_engine();
	return;
}

void write_handler()
{
	#ifdef security_debug
	enable_fetch_lba;
	myprintf("\nLBAw0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	#endif
    //if not continue
     {
	reset_engine_read_write();	//reset_engine and check srst in wait rdy	
	security_check_lba();
  //   	security_check_lba_bak();
    	if(g_current_fis_num != SFR_fis_num)return;

	enable_fetch_lba;
	if((security_mode ==show_master)&&(sata.SUP_LBA3 <0x0A) ||(security_mode ==show_user)&&(sata.SUP_LBA3 >0x09) )
	myprintf("\nLBAw1: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;
	
        #ifdef READ_ONLY 
			#ifdef BUF_CMD_EN
				buf_check_cnt = 0x00;
			#endif
        if(read_only_flag == read_only_enable) 
        {
            SFR_smart_load = smart_load_sata_cnt;
            set_burst_run_wr;
        }
        else 
		#endif
        {
            sata.CHP_cmd = CHP_CMD_WRITE;
            SFR_smart_load = smart_load_chp_cnt | smart_load_sata_cnt;
            SFR_dma_base = 0x0000;
            set_dma_run_wr;
    		g_cont_en = 1;
        }
     }
     
write_cont:
     while(!sata_burst_done)
     {
	    if(g_current_fis_num != SFR_fis_num)return;
	 }
     if(sata_burst_abort)goto write_handler_err;

    #ifdef READ_ONLY
    if(read_only_flag == read_only_enable)
	{
		tx_fis_34(status_good,error_no, int_set);
		goto write_handler_over;
	} 
    #endif
     
     while(!dma_done)
     {
	    if(g_current_fis_num != SFR_fis_num)return;
	 }

     tx_fis_34(status_good,error_no, int_set);
	 if(((sata.CHP0_status_high & 0x80) == 0x80)||((sata.CHP1_status_high & 0x80) == 0x80)||((sata.CHP2_status_high & 0x80) == 0x80)||((sata.CHP3_status_high & 0x80) == 0x80)||((sata.CHP4_status_high & 0x80) == 0x80)) 
     {
        goto write_handler_over;
     }
     //check whether there is continue command.
	 SFR_watch_dog_high = 0x00;
     while(1)
     {
        if(g_current_fis_num != SFR_fis_num)
        {
        	//break;
              //normal_cmd_cont will only be updated when get a new command in RTL.
              //if it's a FIS27 - Control: normal_cmd_cont will stay in 1
            if(normal_cmd_cont && g_cont_en && ((SFR_quick_cmd & quick_cmd_write_dma) != 0x00) )
            {
                SFR_smart_load = smart_load_add_chp_cnt | smart_load_add_sata_cnt;
                sata_burst_cont = 1;
                dma_cont = 1;
                g_current_fis_num = SFR_fis_num;  //we  may  not  go  to  main
                sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
                #ifdef BUF_CMD_EN
				buf_check_cnt = 0x00;
                #endif
                goto write_cont;
            }
            else break;
        }
        if(sata.watch_dog_cnt_low > 0x04) break;
    }
     #ifdef SUPPORT_SMART1
     smart_write_cnt();
	 #endif
     goto write_handler_over;    
         
write_handler_err:
     tx_fis_34(status_bad,error_abort,int_set);
	 #ifdef SUPPORT_SMART1
     updata_smart(smart_crc_num_addr,0x01);
     #endif
write_handler_over:
     //I am not sure host will retry that command immediatelly, need to break the continue mode
	 reset_engine();
     return;
}


/////////////////////////////////////////////////////////////////////
//(1)handle: READ_MULTIPLE, READ_SECTOR, WRITE_MULTIPLE, WRITE_SECTOR
//(2)require PIO setup FIS.
//(3)not enable AUTO mode
//(4)don't cares speed.
/////////////////////////////////////////////////////////////////////
void pio_read_handler()
{
    u8 idata pio_interrput;
    pio_interrput = pio_write_begin;

   security_check_lba();
   
    if(g_flag_lba_err)goto pio_handler_err; 
    
    
    do{   
		reset_engine_read_write();	//reset_engine and check srst in wait rdy	
		if(g_current_fis_num != SFR_fis_num)return;
	
    	//if(g_flag_dir==DIR_WRITE)sata.CHP_cmd = CHP_CMD_WRITE;
    	sata.CHP_cmd = CHP_CMD_READ;
    	SFR_dma_max_num = 0x0001;
        //myprintf("\nchp_en :%xchp_stripe:%x%x",SFR_CHP_en,sata.CHP_stripe_h,sata.CHP_stripe);
        {
           if(g_sec_cnt.word==1)tx_fis_5f(status_drq, error_no, pm_pio_read, g_sec_cnt.byte.l, g_sec_cnt.byte.h, status_good);
           else tx_fis_5f(status_drq, error_no, pm_pio_read, g_sec_cnt.byte.l, g_sec_cnt.byte.h, status_bsy);                 
        }

        
        //to save some code
        //SFR_dma_base = 0x0000;
        SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_conj | SFR_dma_cntl_dir_read;
        
        while(!sata_burst_done)
		{
		   if(g_current_fis_num != SFR_fis_num)return;
		}
        if(sata_burst_abort)goto pio_handler_err;
                
       
        while(!dma_done)
		{
	       if(g_current_fis_num != SFR_fis_num)return;
	    }
			
       
		SFR_dma_cntl =  SFR_dma_cntl_conj;
        g_sec_cnt.word--;
		auto_increase_lba;
                
    }while(g_sec_cnt.word>0);
    sata.FIS_seccnt = g_sec_cnt.byte.l;
	sata.FIS_seccnt_ext = g_sec_cnt.byte.h;
	//smart data handle
	#ifdef SUPPORT_SMART1
    {
	    //smart data handle
	    smart_read_cnt();
    }
	#endif
    return;      
        
pio_handler_err:
	tx_fis_34(status_bad,error_abort,int_set);
    reset_engine();
    #ifdef SUPPORT_SMART1
    updata_smart(smart_crc_num_addr,0x01);
    reset_engine();
    #endif
	return;
}



void pio_write_handler()
{
    u8 idata pio_interrput;
    pio_interrput = pio_write_begin;

    security_check_lba();
	//security_check_lba_bak();
    if(g_flag_lba_err)goto pio_handler_err; 
	//(1)reset_engine and check srst in wait rdy	
	reset_engine_read_write();	
	if(g_current_fis_num != SFR_fis_num)return;

	enable_fetch_lba;
	myprintf("\nPIOw0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;
  
	//(2)write/read command to CHP
	#ifdef READ_ONLY
    if(read_only_flag == read_only_enable) 
	{
		SFR_sata_burst_cnt= 0x0001;
	}
	else
	#endif
    {
        sata.CHP_cmd = CHP_CMD_WRITE;
        SFR_dma_max_num = 0x0001;
    }
    //(3)loop of burst. we only support 512 byte data FIS for one PIO_SETUP FIS  
    do{    
       
        //myprintf("\nchp_en :%xchp_stripe:%x%x",SFR_CHP_en,sata.CHP_stripe_h,sata.CHP_stripe);
        
        {
            tx_fis_5f(status_drq, error_no, pm_pio_write|pio_interrput, g_sec_cnt.byte.l, g_sec_cnt.byte.h, status_bsy);
            pio_interrput = pio_write_next;
        }
        
        #ifdef READ_ONLY
        if(read_only_flag == read_only_enable) SFR_sata_burst_cntl = SFR_sata_burst_cntl_run;
        else 
        #else
		{
        	SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_conj;
		}
        #endif

       
        
        
        while(!sata_burst_done)
		{
		   if(g_current_fis_num != SFR_fis_num)return;
		}
        if(sata_burst_abort)goto pio_handler_err;

        #ifdef READ_ONLY
        if(read_only_flag == read_only_enable) ;
		else
        #endif
        {
           while(!dma_done)
		   {
	          if(g_current_fis_num != SFR_fis_num)return;
	       }
        }
        
		SFR_dma_cntl =  SFR_dma_cntl_conj;
        g_sec_cnt.word--;
		auto_increase_lba;
                
    }while(g_sec_cnt.word>0);
    sata.FIS_seccnt = g_sec_cnt.byte.l;
	sata.FIS_seccnt_ext = g_sec_cnt.byte.h;
	//smart data handle
	#ifdef SUPPORT_SMART1
    {
	    //smart data handle
     	smart_write_cnt();
    }
	#endif
    tx_fis_34(status_good,error_no,int_set);
    return;      
        
pio_handler_err:
	tx_fis_34(status_bad,error_abort,int_set);
    reset_engine();
    #ifdef SUPPORT_SMART1
    updata_smart(smart_crc_num_addr,0x01);
    reset_engine();
    #endif
	return;
}




///////////////////////////////////////////////////////////////////////
//load identify data from code area to SUP 32K FIFO. 
//Then send to SATA host.
///////////////////////////////////////////////////////////////////////


/* 问 题 单: 4     修改人:huawei,   时间:2013/10/28 
   修改原因: 实现双盘备份功调整容量为相同大小为70G/
   /* BEGIN: Added by huawei, 2013/10/28   问题单号:4 */
/*
        SFR_ext_sram_data = 0xC0;
        SFR_ext_sram_data = 0x08;
        tmp1=tmp1&0xFB;//28bits
*/
/* END:   Added by huawei, 2013/10/28 */
void identify_device()
{
	u16 i =0;
	u8 tmp,tmp1;
//	u16_t tmp2;
    
    //clear_dma_run;
    //reset_dma_engine();
        
	//prepare data, read it from code area
	SFR_ext_sram_addr = 0x0000;
	SFR_ext_sram_cntl = 0x10;	//enable auto increese of the sram address
	g_tmp_addr = identify_data_addr;
    for(i=0;i<512;i++)
    {
	    SFR_ext_sram_data = read_code(g_tmp_addr);
        g_tmp_addr ++; 
    }
    
    #ifdef SUPPORT_SECURITY
    if(security_mode == show_master)//70GB
    {
        SFR_ext_sram_addr = 0x0078;
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0xC0;
        SFR_ext_sram_data = 0x08;

        SFR_ext_sram_addr = 0x00C8;  //identify word 100-103
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0xC0;
        SFR_ext_sram_data = 0x08;

	g_tmp_addr = identify_data_addr+0xad;//28bits or 48bits
	tmp1=read_code(g_tmp_addr);
	tmp1=tmp1&0xFB;//28bits
      SFR_ext_sram_addr = 0x00ad;
      SFR_ext_sram_data=tmp1;
    }	
	else if(security_mode == show_user) //70GB
    {
        SFR_ext_sram_addr = 0x0078;
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0xc0;
        SFR_ext_sram_data = 0x08;

        SFR_ext_sram_addr = 0x00C8;  //identify word 100-103
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0x00;
        SFR_ext_sram_data = 0xc0;
        SFR_ext_sram_data = 0x08;

	g_tmp_addr = identify_data_addr+0xad;//28bits or 48bits
	tmp1=read_code(g_tmp_addr);
	tmp1=tmp1&0xFB;//28bits
      SFR_ext_sram_addr = 0x00ad;
      SFR_ext_sram_data=tmp1;
    }	
	
    #endif

	#ifdef security_debug_chs
	g_tmp_addr = identify_data_addr+0x2;//number of logical cylinders
	tmp2.byte.h=read_code(g_tmp_addr);
	g_tmp_addr = identify_data_addr+0x3;//number of logical cylinders
	tmp2.byte.l=read_code(g_tmp_addr);
	myprintf("\ncylinders:%x%x",tmp2.byte.h,tmp2.byte.l);

	g_tmp_addr = identify_data_addr+0x6;//number of logical heads
	tmp2.byte.h=read_code(g_tmp_addr);
	g_tmp_addr = identify_data_addr+0x7;//number of logical heads
	tmp2.byte.l=read_code(g_tmp_addr);
	myprintf("\nheads:%x%x",tmp2.byte.h,tmp2.byte.l);

	g_tmp_addr = identify_data_addr+0xC;//sectors per track
	tmp2.byte.h=read_code(g_tmp_addr);
	g_tmp_addr = identify_data_addr+0xD;//sectors per track
	tmp2.byte.l=read_code(g_tmp_addr);
	myprintf("\nsectors:%x%x",tmp2.byte.h,tmp2.byte.l);
	
	#endif
		
    SFR_ext_sram_addr = 0x0104;   //siganature
    SFR_ext_sram_data = 0x53;  //S
    SFR_ext_sram_data = 0x41;  //A
    SFR_ext_sram_data = 0x47;  //G
    SFR_ext_sram_data = 0x45;  //E
	SFR_ext_sram_cntl = 0x00;	//disable auto increae of the sram address
	
	
	
	
	tmp = gen_check_sum();
	SFR_ext_sram_addr = 0x01ff;
	SFR_ext_sram_data = tmp;

	//send PIO_SETUP_FIS
	tx_fis_5f(status_drq, error_no, pm_pio_read, 0x01, 0x00, status_good);
	//burst it to sata host
	sata_burst(PIO_READ,0x01);
    if(sata_burst_abort)
    {
        tx_fis_34(status_bad,error_abort,int_set);
        reset_engine();
        #ifdef SUPPORT_SMART1
        updata_smart(smart_crc_num_addr,0x01);
        reset_engine();
        #endif
	    return;
    }
}

void set_feature()
{
	BYTE tmp;
	tmp = sata.FIS_seccnt;
	switch(sata.FIS_feature)
	{
	    case enable_write_cache:
		{       
			    g_enable_write_cache = 1;
				tmp = read_code(0x00aa+identify_data_addr);
		        tmp = tmp | 0x20;
			    enable_code_program;
                write_code(0x00aa+identify_data_addr,tmp);  //enable
                disable_code_program;
				break;
		}
		
		case disable_write_cache:
	    {       
				g_enable_write_cache = 0;
				tmp = read_code(0x00aa+identify_data_addr);//and  need  a  flag  here  to  disable  buf_cmd?
		        tmp = tmp & 0xdf;
				enable_code_program;
				write_code(0x00aa+identify_data_addr,tmp);  //disable
			    disable_code_program;
				break;
		}
		
		case mode_transfer:
		{
			if(tmp & mode_ultra_dma) //ultra dma mode
			{
				tmp = tmp & 0x07;
				tmp = 0x01 << tmp;
				enable_code_program;
                write_code(0x00B1+identify_data_addr,tmp);
				write_code(0x007F+identify_data_addr,0x00);
				disable_code_program;
			}
			else if(tmp & mode_mul_dma) //mulitword dma mode
			{
			    tmp = tmp & 0x07;
				tmp = 0x01 << tmp;
				enable_code_program;
                write_code(0x007F+identify_data_addr,tmp);
				write_code(0x00B1+identify_data_addr,0x00);
				disable_code_program;
			}
			else if((tmp & 0xf8)==0)  //default pio mode ;; todo: I don't konw what iordy is ,	so do nothing with it		
		    {
                /*
				enable_code_program;
				write_code(0x0080+identify_data_addr,0x01);//default pio mode is mode 1
				disable_code_program;
				*/

			}
			else if(tmp & mode_pio_tran)// since PIO mode set may disturb host to send PIO read/write only , we close it
			{
			    /*
                         tmp = tmp & 0x07;
				tmp = 0x01 << tmp;
				tmp1= tmp;
				tmp = tmp - 1;
				tmp += tmp1;
				enable_code_program;
                          write_code(0x0080+identify_data_addr,tmp);
				disable_code_program;
				*/
			}
			break;
		}


	    case enable_adv_power_manage:
		{       
				tmp = read_code(0x00ac+identify_data_addr);
		        tmp = tmp | 0x08;
			    enable_code_program;
                write_code(0x00ac+identify_data_addr,tmp);  //enable
                disable_code_program;
				break;
		}

		case disable_adv_power_manage:
		{		
				tmp = read_code(0x00ac+identify_data_addr);
				tmp = tmp & 0xf7;
				enable_code_program;
				write_code(0x00ac+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

		case enable_power_up_standby:
		{		
				tmp = read_code(0x00ac+identify_data_addr);
				tmp = tmp | 0x20;
				enable_code_program;
				write_code(0x00ac+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

		case disable_power_up_standby:
		{		
				tmp = read_code(0x00ac+identify_data_addr);
				tmp = tmp & 0xdf;
				enable_code_program;
				write_code(0x00ac+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

		case enable_rw_verify:
		{		
				tmp = read_code(0x00f0+identify_data_addr);
				tmp = tmp | 0x02;
				enable_code_program;
				write_code(0x00f0+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

        case disable_rw_verify:
		{		
				tmp = read_code(0x00f0+identify_data_addr);
				tmp = tmp & 0xfd;
				enable_code_program;
				write_code(0x00f0+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

		case enable_look_ahead:
		{		
				tmp = read_code(0x00aa+identify_data_addr);
				tmp = tmp | 0x40;
				enable_code_program;
				write_code(0x00aa+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

        case disable_look_ahead:
		{		
				tmp = read_code(0x00aa+identify_data_addr);
				tmp = tmp & 0xbf;
				enable_code_program;
				write_code(0x00aa+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

		case enable_auto_acoustic_manage:
		{		
				tmp = read_code(0x00ad+identify_data_addr);
				tmp = tmp | 0x02;
				enable_code_program;
				write_code(0x00ad+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

        case disable_auto_acoustic_manage:
		{		
				tmp = read_code(0x00ad+identify_data_addr);
				tmp = tmp & 0xfd;
				enable_code_program;
				write_code(0x00ad+identify_data_addr,tmp);	//enable
				disable_code_program;
				break;
		}

		case enable_revert_pow_default:
		{		
				break;
		}		

        case disable_revert_pow_default:
		{		
				break;
		}

		case disable_sata_feature:
		{
			    break;
		}
		default :
		{
			    tx_fis_34(status_bad,error_abort,int_set);
				return;
		}		
	}
	tx_fis_34(status_good,error_no,int_set);
}

void init_dev_para()
{
	u8 idata tmp8;
	if((sata.fis_rcv_content[7] & 0x0F) != (max_head_per_cylnd-1))goto init_dev_para_err;
	if(sata.fis_rcv_content[12]!=max_sec_per_head)goto init_dev_para_err;
	tmp8 = read_code(0x006A+identify_data_addr);
	//The device shall also clear bit 0 of word 53 in the IDENTIFY DEVICE data to zero
	tmp8 = tmp8 | 0x01;
	enable_code_program;
    write_code(0x006A+identify_data_addr,tmp8);
	disable_code_program;
	tx_fis_34(status_good,error_no,int_set);
	return;
init_dev_para_err:
	tmp8 = read_code(0x006A+identify_data_addr);
	//The device shall also clear bit 0 of word 53 in the IDENTIFY DEVICE data to zero
	tmp8 = tmp8 & 0xfe;
	enable_code_program;
    write_code(0x006A+identify_data_addr,tmp8);
	disable_code_program;
	tx_fis_34(status_bad,error_abort,int_set);

}

#ifdef SUPER_ERASE

/* 问 题 单: 4     修改人:huawei,   时间:2013/10/28 
   修改原因: 实现双盘备份功调整容量为相同大小为70G/
   /* BEGIN: Added by huawei, 2013/10/28   问题单号:4 */
/*
      sum.Number=0x08c00000;
       sata.SUP_LBA2 = 0xc0;
	 sata.SUP_LBA3 = 0x08;
	 if(security_mode ==show_master)
	{
	 sata.SUP_LBA3 = sum.No[0]+0x08;
	 sata.SUP_LBA2 = sum.No[1]+0xc0;
	 }
	else
	 {
	  sata.SUP_LBA3 = sum.No[0];
	  sata.SUP_LBA2 = sum.No[1];
	  }	
*/
/* END:   Added by huawei, 2013/10/28 */
void intial_data_partition(u8 all_partial)
{
    u8 ii;
    if(all_partial)//erase all,do not care  start_adress and end_address
    	{
		myprintf("\n ERASE ALL 2");
	g_tmp_addr = identify_data_addr+0xad;//28bits or 48bits
	if((read_code(g_tmp_addr)&(0x04))==0x04) g_tmp_addr = identify_data_addr+0xc8;
	else  g_tmp_addr = identify_data_addr+0x78;//word 60-61
	for(ii=0;ii<4;ii++)
	{
	 sum.No[3-ii]= read_code(g_tmp_addr);
        g_tmp_addr ++;
	} 

	sata.SUP_LBA0 = 0x00;
	sata.SUP_LBA1 = 0x00;
	sata.SUP_LBA2 = 0x00;
	sata.SUP_LBA3 = 0x00;
	sata.SUP_LBA4 = 0x00;
	sata.SUP_LBA5 = 0x00;


	send_trim_cmd(sum);

	g_tmp_addr = identify_data_addr+0xad;//28bits or 48bits
	if((read_code(g_tmp_addr)&(0x04))==0x04) g_tmp_addr = identify_data_addr+0xc8;
	else 
		g_tmp_addr = identify_data_addr+0x78;//word 60-61
	for(ii=0;ii<4;ii++)
	{
	    sum.No[3-ii]= read_code(g_tmp_addr);
        g_tmp_addr ++;
	} myprintf("\nerase all");
	
     }
     else //erase partial
     	{
     	 if(security_erase==show_user)
     	 	{

		sata.SUP_LBA0 = 0x00;
		sata.SUP_LBA1 = 0x00;
		sata.SUP_LBA2 = 0x00;
		sata.SUP_LBA3 = 0x00;
		sata.SUP_LBA4 = 0x00;
		sata.SUP_LBA5 = 0x00;
		
		sum.Number=0x08c00000;
		send_trim_cmd(sum);	
		sum.Number=0x08c00000;

		myprintf("\nerase p1");
		
     	 	}
	 else if(security_erase==show_master)
	 	{
             	sata.SUP_LBA0 = 0x00;
	 	sata.SUP_LBA1 = 0x00;
	  	sata.SUP_LBA2 = 0xc0;
	  	sata.SUP_LBA3 = 0x08;
	  	sata.SUP_LBA4 = 0x00;
	  	sata.SUP_LBA5 = 0x00;
		
		sum.Number=0x08C00000;
		send_trim_cmd(sum);	
		sum.Number=0x08C00000;

		myprintf("\nerase p2");
		
	     }
		
	 }
	
	SFR_ext_sram_addr = 0x0000;	
	SFR_ext_sram_cntl = 0x10;
	while(SFR_ext_sram_addr<0x8000)SFR_ext_sram_data = 0x77;
	
	trim_addr_offset.No[3] = BLOCK_SIZE_L;
	trim_addr_offset.No[2] = BLOCK_SIZE_H;
	trim_addr_offset.No[1] = 0x00;
	trim_addr_offset.No[0] = 0x00;
	trim_addr_offset.Number =trim_addr_offset.Number<<3;
	sum.Number -= trim_addr_offset.Number;
	g_sec_cnt.word = trim_addr_offset.Word[1];
	myprintf("sec:%x %x",g_sec_cnt.word);
	myprintf("\nnum:%x %x %x %x",sum.No[3],sum.No[2],sum.No[1],sum.No[0]);
	
	reset_engine();
	sata.SUP_LBA0 = sum.No[3];
	sata.SUP_LBA1 = sum.No[2];
	//sata.SUP_LBA2 = sum.No[1];
	
	if(security_mode ==show_master)
	{
	 sata.SUP_LBA3 = sum.No[0]+0x08;
	 sata.SUP_LBA2 = sum.No[1]+0xc0;
	 }
	else
	 {
	  sata.SUP_LBA3 = sum.No[0];
	  sata.SUP_LBA2 = sum.No[1];
	  }	
	
	sata.SUP_LBA4 = 0x00;
	sata.SUP_LBA5 = 0x00;
	sata.CHP_cmd = CHP_CMD_WRITE;
	SFR_dma_base = 0x0000;

    if((g_sec_cnt.byte.l != 0x00)||(g_sec_cnt.byte.h != 0x00))//sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_set_dma_cnt_high;
    SFR_dma_max_num = g_sec_cnt.word;
	else 
	{
	    SFR_dma_max_num = 0x0000;
	    sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_set_dma_cnt_high;
	    trim_addr_offset.Word[0] -= 0x01;
	}
    SFR_dma_cntl = SFR_dma_cntl_run;
	while(!dma_done);

	while(trim_addr_offset.Word[0]>0)
	{
	    SFR_dma_max_num = 0x0000;
	    sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_set_dma_cnt_high;
		dma_cont = 1;
		while(!dma_done);
	}

	
    clear_dma_run;    
	
	SFR_ext_sram_cntl = 0x00; 
	flush_delay();
	return;
}

#endif
	
#ifdef READ_ONLY

void choose_mode()
{
    switch(sata.FIS_feature)
    {
        case read_only_enable:
        {
            chp_burst(CHP_CMD_READ,0x01,block_base_log);
            SFR_ext_sram_addr = 0x0007;
            SFR_ext_sram_cntl = 0x00;
            SFR_ext_sram_data = read_only_enable;
            chp_burst(CHP_CMD_WRITE,0x01,block_base_log);
			flush_delay();
            read_only_flag = read_only_enable;
            tx_fis_34(status_good,error_no,int_set);
            break;
        }
        case read_only_disable:
        {
            chp_burst(CHP_CMD_READ,0x01,block_base_log);
            SFR_ext_sram_addr = 0x0007;
            SFR_ext_sram_cntl = 0x00;
            SFR_ext_sram_data = read_only_disable;
            chp_burst(CHP_CMD_WRITE,0x01,block_base_log);
			flush_delay();
            read_only_flag = read_only_disable;
            tx_fis_34(status_good,error_no,int_set);
            break;
        }
        default: 
		    tx_fis_34(status_bad,error_abort,int_set);
			break;
    }
    reset_engine();
    return;
}

#endif
