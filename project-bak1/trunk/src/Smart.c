#ifdef SUPPORT_SMART

void smart_handler()
{
	//myprintf("\nsmart");
	g_smart_read_flag = 0;
    if(g_smart_operation ||(sata.FIS_feature== smart_enable_operation))    //if enable smart operation
    {
        switch(sata.FIS_feature)
        {
            //this command disables all smart operations. smart data shall no longe be mnitored or saved by the dvice.
            //the state of SMART, eithe enabled or disabled, shall be preserved by the device accross power cycles.
            //after receipt of this command, all SMART command except for smart_enable_operations shall be disable.
            case smart_disable_operation:		//have not appeared
            {
				//myprintf("\ndisable_smart");
                g_smart_operation = 0;
                break;
            }
            case smart_enable_operation:		//ok
            {
				//myprintf("\nenable_smart");
                g_smart_operation = 1;
                break;
            }  
            case smart_return_status:			//ok
            {
				//myprintf("\nsmart_status");
                smart_status();
                break;
            }
            case smart_attribute_autosave:		//not completely.have not appeared.
            {
				//myprintf("\nsmart_autosave");
                if(sata.FIS_seccnt == smart_attribute_autosave_dis)
                {
                	//myprintf("\ndisable_autosave");
                    g_smart_attribute_autosave = 0;                
                }
                else if(sata.FIS_seccnt == smart_attribute_autosave_en)
                {
                	//myprintf("\nenable_autosave");
                    g_smart_attribute_autosave = 1;
                }
                break;
            }
               
            case smart_execute_offline:	//optional,have not appeared
            {
				//myprintf("\nsmart_offline");
                smart_offline();
                break;
            }
            case smart_read_data:		//ok
            {
                //myprintf("\nsmart_attribute");
                smart_data_handler(smart_attribute,0x01,CHP_CMD_READ);
				g_smart_read_flag = 1;
				break;
            }
			
            case smart_read_threshold:	//ok
            {
                //myprintf("\nsmart_threshold");
                smart_data_handler(smart_threshold,0x01,CHP_CMD_READ);
				g_smart_read_flag = 1;
				break;
            }
            
            case smart_read_log:		//have not appeared
            {
                //goto smart_handler_err;
                //myprintf("\nsmart_read_log");
                smart_log(CHP_CMD_READ);
				g_smart_read_flag = 1;
				break;
            }
            
            case smart_write_log:		//If the host attempts to write to a read only log address, the device shall return command aborted.
            {							//have not appeared
                //goto smart_handler_err; 
                //myprintf("\nsmart_write_log");
                smart_log(CHP_CMD_WRITE);
                break;
            }
            default:
            {
                g_smart_abort = 1;
                
            }            
        }  
        if(g_smart_abort) goto smart_handler_err;
        if(!g_smart_read_flag)tx_fis_34(status_good, error_no, int_set); 
        return;
    }
smart_handler_err:
    g_smart_abort = 0;
    tx_fis_34(status_bad,error_abort,int_set);
    reset_engine();
    #ifdef SUPPORT_SMART1
    updata_smart(smart_crc_num_addr,0x01);
    reset_engine();
    #endif
	return;
}
void smart_offline()					//i don't know what is short,cnveyance,selective self test routine.
{
	switch(sata.fis_rcv_content[4*1+0])
	{
		case offline_routine:
		{
			//off_line_data_collection();//The results of this routine are placed in the Off-line data collection status byte.
			break;
		}
		case short_self_test_offline:	//optional
		{
			g_smart_offline_flag = 1;
			break;
		}
		case self_test_offline_ext:		//optional
		{
			g_smart_offline_flag = 1;
			break;
		}
		case conveyance_self_test_offline://optional
		{
			g_smart_offline_flag = 1;
			break;
		}
		case selective_self_test_offline:	//optional
		{
			break;
		}
		case abort_self_test_offline:		//optional
		{
			g_smart_offline_flag = 1;
			g_smart_test_int = 1;
			break;
		}
		case short_self_test_captive:		//optional
		{
			g_smart_offline_flag = 1;
			break;
		}
		case self_test_captive_ext:			//optional
		{
			g_smart_offline_flag = 1;
			break;
		}
		case conveyance_self_test_captive:		//optional
		{
			g_smart_offline_flag = 1;
			break;
		}
		case selective_self_test_captive:		//optional
		{
			break;
		}
	}
    
}
void smart_log(u8 chp_cmd)
{
	g_smart_abort = 0;
	//(1)write log
	if(chp_cmd == CHP_CMD_WRITE)
	{
		if(sata.fis_rcv_content[4*1+0] == selective_self_text_log)
		{
			smart_data_handler(smart_selective_self_text_log,0x01,chp_cmd);	
		}
		else
		{
			g_smart_abort = 1;
		}
		
		goto smart_log_over;
	}
    //(2)read block information
	if(sata.fis_rcv_content[4*1+0] >= (block_base_log - 0x06)) 
    {
        #ifdef SUPPORT_ENCRY
        if(sata.fis_rcv_content[4*1+0] == password_addr)
        {
            g_smart_abort = 1;
            goto smart_log_over;
        }
        #endif
        smart_data_handler(sata.fis_rcv_content[4*1+0],0x01,chp_cmd);
        goto smart_log_over;
    }
    //(3)read log
    switch(sata.fis_rcv_content[4*1+0])        //check log_address,lba0.
    {
        case log_directory:
        {
            smart_data_handler(smart_log_directory,0x01,chp_cmd);
            break;
        }
        case summary_error_log:
        {
            smart_data_handler(smart_summary_error_log,0x01,chp_cmd);	
            break;
        }
        case comprehensive_error_log:
        {
            smart_data_handler(smart_comprehensive_error_log,0x01,chp_cmd);	////The maximum size of the Comprehensive SMART Error log shall be 51 log pages.so 0x01 need to be modified.
            break;
        }
        case self_test_log:
        {
            smart_data_handler(smart_self_test_log,0x01,chp_cmd);
            break;
        }
        case selective_self_text_log:	//it is connect with offline
        {
            smart_data_handler(smart_selective_self_text_log,0x01,chp_cmd);	
            break;
        }
        default:
        {
            g_smart_abort = 1;        //find a log address that no defined
            break;
        }
    }
smart_log_over:
	return;
	
}
void smart_status()
{
	sata.FIS_lba_mid = 0x4f;
	sata.FIS_lba_high = 0xc2;	
}
void smart_data_handler(u8 base_sec,u8 sec_cnt,u8 chp_cmd)
{
    //fetch lba
    
    u8 pio_interrput;
	u8 cur_cnt = 0x00;
	pio_interrput = pio_write_begin;
	
	g_smart_abort = 0;

	while(sec_cnt > 0x00)
	{
		
        if(chp_cmd == CHP_CMD_WRITE)
        {
        	//myprintf("\nsmart_write");
            tx_fis_5f(status_drq, error_no, pm_pio_write|pio_interrput, sec_cnt, 0x00, status_bsy);
            pio_interrput = pio_write_next;
            //set_dma_run_wr;
            sata_burst(PIO_WRITE,0x01);
			if(sata_burst_abort)g_smart_abort = 1;
			
			chp_burst(chp_cmd,0x01,cur_cnt+base_sec);
        }
        else
        {
        	//myprintf("\nsmart_read");
            if(sec_cnt == 1) tx_fis_5f(status_drq, error_no, pm_pio_read, sec_cnt, 0x00, status_good);
            else tx_fis_5f(status_drq, error_no, pm_pio_read, sec_cnt, 0x00, status_bsy);  
            //set_dma_run_rd;
    		         
			chp_burst(chp_cmd,0x01,cur_cnt+base_sec);
			if((g_smart_offline_flag == 1) && (base_sec == smart_attribute))	self_test_routine();
			sata_burst(PIO_READ,0x01);
			if(sata_burst_abort)g_smart_abort = 1;
            
        }
		cur_cnt += 0x01;
		sec_cnt -= 0x01;
		//myprintf("\ncur:%x, sec:%x",cur_cnt,sec_cnt);
		//if(cur_cnt==sec_cnt)break;
	}
    reset_engine();
}




void self_test_routine()
{
	u8 tmp;
    //SFR_ext_sram_cntl = 0x00;
	SFR_ext_sram_addr = 0x016b;
	if(g_smart_test_int == 1)
	{
		SFR_ext_sram_data = 0x10;
		//g_smart_test_int = 0;
     	goto self_test_over;
	}
	switch(g_smart_test_status)
	{
		case	0x00:	
		{
			SFR_ext_sram_data = 0xf9;	//363:Self-test execution status byte
			break;
		}
		case	0x01:
		{
			SFR_ext_sram_data = 0xf8;
            break;
		}
		case	0x02:
		{
			SFR_ext_sram_data = 0xf7;
            break;
		}
		case	0x03:
		{
			SFR_ext_sram_data = 0xf6;
            break;
		}
		case	0x04:
		{
			SFR_ext_sram_data = 0xf5;
            break;
		}
		case	0x05:
		{
			SFR_ext_sram_data = 0xf4;
            break;
		}
		case	0x06:
		{
			SFR_ext_sram_data = 0xf3;
            break;
		}
		case	0x07:
		{
			SFR_ext_sram_data = 0xf2;
            break;
		}
		case	0x08:
		{
			SFR_ext_sram_data = 0xf1;
            break;
		}
		case	0x09:	
		{
			SFR_ext_sram_data = 0x00;
            break;
		}
	}
self_test_over:
	SFR_ext_sram_cntl = 0x00;
	tmp = gen_check_sum();
	SFR_ext_sram_addr = 0x01ff;
	SFR_ext_sram_data = tmp;
	g_smart_test_status++;
	if((g_smart_test_status == 0x0a) || (g_smart_test_int == 1)) 
	{
		g_smart_test_status = 0x00;
		g_smart_offline_flag = 0;
        g_smart_test_int = 0;
	}
}

/*
void off_line_data_collection()
{

}
*/

/*
void arrtibute_byte_handler()
{
	if(Off-line data collection activity was completed without error.)    //
	{
		smart_read_chp(smart_attribute);
		SFR_ext_sram_addr = 0x016a;
		SFR_ext_sram_data = 0x82;
		smart_write_chp(smart_attribute);
	}
	if(Off-line data collection activity was suspended by an interrupting command from host.)    //
	{
		smart_read_chp(smart_attribute);
		SFR_ext_sram_addr = 0x016a;
		SFR_ext_sram_data = 0x84;
		smart_write_chp(smart_attribute);
	}
	if(Off-line data collection activity was aborted by an interrupting command from host.)    //
	{
		smart_read_chp(smart_attribute);
		SFR_ext_sram_addr = 0x016a;
		SFR_ext_sram_data = 0x85;
		smart_write_chp(smart_attribute);
	}
	if(Off-line data collection activity was aborted by the device with a fatal error.)    //
	{
		smart_read_chp(smart_attribute);
		SFR_ext_sram_addr = 0x016a;
		SFR_ext_sram_data = 0x86;
		smart_write_chp(smart_attribute);
	}
}
*/



//CHP
void updata_smart(u8 smart_addr, u8 smart_cnt)
{
	u8 tmp;
	u8 tmp_data; //the old ext-sram-data
	reset_engine();
	chp_burst(CHP_CMD_READ,0x01,smart_attribute);
	SFR_ext_sram_cntl = 0x00;
	
	//(1)byte 0
	SFR_ext_sram_addr = smart_addr;
	tmp_data = SFR_ext_sram_data;
	SFR_ext_sram_data += smart_cnt;
	
	//(2)byte  1
	if(tmp_data > SFR_ext_sram_data)
	{
		SFR_ext_sram_addr += 0x01;
		tmp_data = SFR_ext_sram_data;
		SFR_ext_sram_data += 0x01;
		//(3)byte 2
		if(tmp_data > SFR_ext_sram_data)
		{
			SFR_ext_sram_addr += 0x01;
			tmp_data = SFR_ext_sram_data;
			SFR_ext_sram_data += 0x01;
			//(4)byte 3
			if(tmp_data > SFR_ext_sram_data)
			{
				SFR_ext_sram_addr += 0x01;
				tmp_data = SFR_ext_sram_data;
				SFR_ext_sram_data += 0x01;
			}
		}
	}
	tmp = gen_check_sum();
	SFR_ext_sram_addr = 0x01ff;
	SFR_ext_sram_data = tmp;
	chp_burst(CHP_CMD_WRITE,0x01,smart_attribute);
	reset_engine();
}

void updata_smart_n(u8 smart_addr, u8 smart_cnt)
{
    u8 tmp;
	u8 tmp_data; //the old ext-sram-data
	reset_engine();
	chp_burst(CHP_CMD_READ,0x01,smart_attribute);
	SFR_ext_sram_cntl = 0x00;
	
	//(1)byte 0
	SFR_ext_sram_addr = smart_addr;
	tmp_data = SFR_ext_sram_data;
	SFR_ext_sram_data -= smart_cnt;
	
	//(2)byte  1
	if(tmp_data < SFR_ext_sram_data)
	{
		SFR_ext_sram_addr += 0x01;
		tmp_data = SFR_ext_sram_data;
		SFR_ext_sram_data -= 0x01;
		//(3)byte 2
		if(tmp_data < SFR_ext_sram_data)
		{
			SFR_ext_sram_addr += 0x01;
			tmp_data = SFR_ext_sram_data;
			SFR_ext_sram_data -= 0x01;
			//(4)byte 3
			if(tmp_data < SFR_ext_sram_data)
			{
				SFR_ext_sram_addr += 0x01;
				tmp_data = SFR_ext_sram_data;
				SFR_ext_sram_data -= 0x01;
			}
		}
	}
	tmp = gen_check_sum();
	SFR_ext_sram_addr = 0x01ff;
	SFR_ext_sram_data = tmp;
	chp_burst(CHP_CMD_WRITE,0x01,smart_attribute);
	reset_engine();
}

void smart_write_cnt()
{
    u16 tmp;
	g_smart_write_handle = 1;
	//myprintf("\nwrite");
	tmp = write_sec_cnt_l;
	write_sec_cnt_l += SFR_SATA_done_num;
    write_sec_cnt_h += SFR_SATA_done_num_h;
    if(write_sec_cnt_l <= tmp) write_sec_cnt_h += 0x01;
	while(write_sec_cnt_h >= 0x20)
	{
		write_sec_cnt_h -= 0x20;
		g_smart_write_cnt += 0x01;
	}
}

//Note:don't print in this function!!
void smart_read_cnt()
{
    u16 tmp;
	//myprintf("\nsmart");myprintf("\nsmart");myprintf("\nsmart");myprintf("\nsmart");
	g_smart_read_handle = 1;
    tmp = read_sec_cnt_l;
	read_sec_cnt_l += SFR_SATA_done_num;
    read_sec_cnt_h += SFR_SATA_done_num_h;
    if(read_sec_cnt_l <= tmp) read_sec_cnt_h += 0x01;
	while(read_sec_cnt_h >= 0x20)
	{
		read_sec_cnt_h -= 0x20;
		g_smart_read_cnt += 0x01;
	}
}

//Note:don't print in this function!!
void smart_block_handle()
{
    u8 chp_sel;
    u8 block_infor0;
    u8 block_infor1;
    u8 block_infor2;
    u8 block_infor3;
    u8 block_infor4;
    u8 block_infor5;
    u8 block_infor6;
check_loop:
	if(DEF_CHP_EN & 0x01)	{ if((sata.CHP0_status_high & 0x80) == 0x80)	{chp_sel = 0x01;sata.CHP_stripe = 0x10;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x02)	{ if((sata.CHP1_status_high & 0x80) == 0x80)	{chp_sel = 0x02;sata.CHP_stripe = 0x20;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x04)	{ if((sata.CHP2_status_high & 0x80) == 0x80)	{chp_sel = 0x04;sata.CHP_stripe = 0x40;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x08)	{ if((sata.CHP3_status_high & 0x80) == 0x80)	{chp_sel = 0x08;sata.CHP_stripe = 0x80;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x10)	{ if((sata.CHP4_status_high & 0x80) == 0x80)	{chp_sel = 0x10;sata.CHP_stripe = 0x00;sata.CHP_stripe_h = 0x10;goto handle;}}
	goto end_loop;
handle:
    reset_engine();
    SFR_CHP_en = chp_sel;
    stop_all_chp;
    wait_chp_ready();
	SFR_dma_base = 0x0000;
	SFR_dma_max_num_l = 0x01;
    SFR_dma_max_num_h = 0x00;
    sata.CHP_cmd = CHP_CMD_BLOCK;
	SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_dir_read;
	while(!dma_done);
	wait_chp_ready(); 
	clear_dma_run;
    //save block information
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x0000;
    block_infor0 = SFR_ext_sram_data;
    block_infor1 = SFR_ext_sram_data;
    block_infor2 = SFR_ext_sram_data;
    block_infor3 = SFR_ext_sram_data;
    block_infor4 = SFR_ext_sram_data;
    block_infor5 = SFR_ext_sram_data;
    block_infor6 = SFR_ext_sram_data;
    //open all chp
    sata.CHP_stripe = DEF_CHP_STRIPE;
    sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
    
    //read information addr
    chp_burst(CHP_CMD_READ,0x01,block_base_log);
    SFR_ext_sram_addr = 0x0000;
    SFR_ext_sram_cntl = 0x10;
    g_smart_block_cnt_h = SFR_ext_sram_data;//the page of block table 
    g_smart_block_cnt_l = SFR_ext_sram_data;//the addr in one block table
    SFR_ext_sram_cntl = 0x00;
	
    //add addr
    if(g_smart_block_cnt_l == 0xff)
    {
        g_smart_block_cnt_h = 0x00;
        g_smart_block_cnt_l = 0x00;
    }
	
    g_smart_block_cnt_l += 0x01;
    if(g_smart_block_cnt_l == 0x40) 
    {
        g_smart_block_cnt_h += 0x01;
        g_smart_block_cnt_l = 0x00;
    }
    if(g_smart_block_cnt_h == 0x30) g_smart_block_cnt_h = 0x00;
	
    //save information addr
    SFR_ext_sram_addr = 0x0000;
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_data = g_smart_block_cnt_h;
    SFR_ext_sram_data = g_smart_block_cnt_l;
    SFR_ext_sram_cntl = 0x00;
    chp_burst(CHP_CMD_WRITE,0x01,block_base_log);

    //read smart information 
    chp_burst(CHP_CMD_READ,0x01,block_base_log + g_smart_block_cnt_h);
    //save smart information
    SFR_ext_sram_addr = 0x0000 + 8*g_smart_block_cnt_l;
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_data = chp_sel;
    SFR_ext_sram_data = block_infor0;
    SFR_ext_sram_data = block_infor1;
    SFR_ext_sram_data = block_infor2;
    SFR_ext_sram_data = block_infor3;
    SFR_ext_sram_data = block_infor4;
    SFR_ext_sram_data = block_infor5;
    SFR_ext_sram_data = block_infor6;
	SFR_ext_sram_cntl = 0x00;
	chp_burst(CHP_CMD_WRITE,0x01,block_base_log + g_smart_block_cnt_h);
    updata_smart(smart_ecc_num_addr,0x01);
    if(block_infor6 == 0x80) 
    {
    	updata_smart(smart_block_error_addr,0x01);
    }
    if(block_infor6 == 0x04) 
    {
        updata_smart(smart_block_fail,0x01);
        updata_smart_n(smart_ecc_num_addr,0x01);
    }
    if(block_infor6 == 0x10)
    {
        updata_smart_n(smart_ecc_num_addr,0x01);
    }
    reset_engine();
    sata.CHP_stripe = DEF_CHP_STRIPE;
    sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
	SFR_watch_dog_high = 0x00;
	goto check_loop;
end_loop:
	return; 
}
void smart_card_update()
{
    u8 tmp;
    u8 card_infor0;
    u8 card_infor1;
    u8 card_infor2;
	u8 card_infor3;
    u8 card_infor40;
    u8 card_infor41;
	u8 card_infor42;
	u8 card_infor43;
	
    reset_engine();

	//enable one channal only
	SFR_CHP_en = 0x01;
	sata.CHP_stripe = 0x10;
	sata.CHP_stripe_h = 0x00;
    //handle
    SFR_dma_base = 0x0000;
    SFR_dma_max_num_l = 0x04;
    SFR_dma_max_num_h = 0x00;
    sata.CHP_cmd = CHP_CMD_ERASE_CNT;
    SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_dir_read;
    while(!dma_done);
    clear_dma_run;
    wait_chp_ready();
    //save information
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x0000;
    card_infor0  = SFR_ext_sram_data;
    card_infor1  = SFR_ext_sram_data;
    card_infor2  = SFR_ext_sram_data;
	card_infor3  = SFR_ext_sram_data;
    card_infor40 = SFR_ext_sram_data;
    card_infor41 = SFR_ext_sram_data;
	card_infor42 = SFR_ext_sram_data;
	card_infor43 = SFR_ext_sram_data;
    SFR_ext_sram_cntl = 0x00;

    SFR_CHP_en = DEF_CHP_EN;
	sata.CHP_stripe = DEF_CHP_STRIPE;
	sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
	
    //read smart information
    chp_burst(CHP_CMD_READ,0x01,smart_attribute);
    //save smart information
    SFR_ext_sram_cntl = 0x10;
    
    SFR_ext_sram_addr = smart_erase_addr;
    SFR_ext_sram_data = card_infor0;
    
    SFR_ext_sram_addr = smart_program_fail;
    SFR_ext_sram_data = card_infor1;
    
    SFR_ext_sram_addr = smart_block_fail;
    SFR_ext_sram_data = card_infor2;
    
    SFR_ext_sram_addr = smart_card_infor3;
	SFR_ext_sram_data = card_infor3;

    
    SFR_ext_sram_addr = smart_card_infor4;
    SFR_ext_sram_data = card_infor40;
	SFR_ext_sram_data = card_infor41;
	SFR_ext_sram_data = card_infor42;
	SFR_ext_sram_data = card_infor43;
	SFR_ext_sram_cntl = 0x00;
   
    tmp = gen_check_sum();
    SFR_ext_sram_addr = 0x01ff;
    SFR_ext_sram_data = tmp;
    chp_burst(CHP_CMD_WRITE,0x01,smart_attribute);
    //end handle
    reset_engine();
    return;

}

void smart_erase_handle()
{
    u8 tmp;
    u8 chp_sel;
    u8 erase_infor0;
    u8 erase_infor1;
    u8 erase_infor2;
    u8 erase_infor3;
    u32_t pe;
    u32_t erase;
    u8 cnt;
    chp_sel = 0x01;
    reset_engine();
handle:
    SFR_CHP_en = chp_sel;
    if(chp_sel == 0x01) {sata.CHP_stripe = 0x10;sata.CHP_stripe_h = 0x00;}
    if(chp_sel == 0x02) {sata.CHP_stripe = 0x20;sata.CHP_stripe_h = 0x00;}
    if(chp_sel == 0x04) {sata.CHP_stripe = 0x40;sata.CHP_stripe_h = 0x00;}
    if(chp_sel == 0x08) {sata.CHP_stripe = 0x80;sata.CHP_stripe_h = 0x00;}
    if(chp_sel == 0x10) {sata.CHP_stripe = 0x00;sata.CHP_stripe_h = 0x10;}
    stop_all_chp;
    wait_chp_ready();
    SFR_dma_base = 0x0000;
	SFR_dma_max_num_l = 0x04;
    SFR_dma_max_num_h = 0x00;
    sata.CHP_cmd = CHP_CMD_ERASE_CNT;
	SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_dir_read;
    while(!dma_done);
    clear_dma_run;
	wait_chp_ready();
    //save information
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x0160;
    erase_infor0 = SFR_ext_sram_data;
    erase_infor1 = SFR_ext_sram_data;
    erase_infor2 = SFR_ext_sram_data;
    erase_infor3 = SFR_ext_sram_data;  
    //begin to deal data
    sata.CHP_stripe = DEF_CHP_STRIPE;
    sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
    ///////////////////////////////////////////////////////
    //for ssd left time
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x0010;
    //"0" is high
    erase.No[3] = SFR_ext_sram_data;
    erase.No[2] = SFR_ext_sram_data;
    erase.No[1] = SFR_ext_sram_data;
    erase.No[0] = SFR_ext_sram_data;
    //myprintf("\nerase.No[3]:%x",erase.No[3]);
    //myprintf("\nerase.No[2]:%x",erase.No[2]);
    //myprintf("\nerase.No[1]:%x",erase.No[1]);
    //myprintf("\nerase.No[0]:%x",erase.No[0]);
    
    pe.No[3] = SMART_PE_0;
    pe.No[2] = SMART_PE_1;
    pe.No[1] = SMART_PE_2;
    pe.No[0] = SMART_PE_3;
    //myprintf("\npe.No[3]:%x",pe.No[3]);
    //myprintf("\npe.No[2]:%x",pe.No[2]);
    //myprintf("\npe.No[1]:%x",pe.No[1]);
    //myprintf("\npe.No[0]:%x",pe.No[0]);
    if(pe.Number != 0)
    {
        cnt = 0x00;
        while(erase.Number >= pe.Number)
        {
            cnt += 0x01;
            erase.Number = erase.Number - pe.Number;
        }
        //myprintf("\ncnt:%x",cnt);
        chp_burst(CHP_CMD_READ,0x01,smart_attribute);
        SFR_ext_sram_cntl = 0x00;
        SFR_ext_sram_addr = smart_left_time;
        SFR_ext_sram_data = smart_100 - cnt;
        if(SFR_ext_sram_data > smart_100) SFR_ext_sram_data = 0x00;
        chp_burst(CHP_CMD_WRITE,0x01,smart_attribute);
    }
    ////////////////////////////////////////////////
    //read smart information
    chp_burst(CHP_CMD_READ,0x01,smart_attribute);
    //save smart information
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = smart_erase_addr;
    if(chp_sel != 0x01)     //not only chp0
    {
        //byte 0
        tmp = erase_infor0;
        erase_infor0 = tmp + SFR_ext_sram_data;
        if(tmp > erase_infor0) erase_infor1 += 0x01;
        //byte 1
        tmp = erase_infor1;
        erase_infor1 = tmp + SFR_ext_sram_data;
        if(tmp > erase_infor1) erase_infor2 += 0x01;
        //byte 2
        tmp = erase_infor2;
        erase_infor2 = tmp + SFR_ext_sram_data;
        if(tmp > erase_infor2) erase_infor3 += 0x01;
        //byte 3
        tmp = erase_infor3;
        erase_infor3 = tmp + SFR_ext_sram_data;
        if(tmp > erase_infor3) erase_infor3 = 0xff;

        SFR_ext_sram_addr = smart_erase_addr;
    }
    SFR_ext_sram_data = erase_infor0;
    SFR_ext_sram_data = erase_infor1;
    SFR_ext_sram_data = erase_infor2;
    SFR_ext_sram_data = erase_infor3;
    SFR_ext_sram_cntl = 0x00;
    tmp = gen_check_sum();
	SFR_ext_sram_addr = 0x01ff;
	SFR_ext_sram_data = tmp;
	chp_burst(CHP_CMD_WRITE,0x01,smart_attribute);
    chp_sel = chp_sel << 1;
    if((DEF_CHP_EN & chp_sel) == 0x00) goto end_handle;
    goto handle;
end_handle:
    sata.CHP_stripe = DEF_CHP_STRIPE;
    sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
    reset_engine();
    return;
}



void smart_card_crc()
{
    u8 chp_sel;
    u8 tmp;
    u8 idata block_infor0;
    u8 idata block_infor1;
    u8 idata block_infor2;
    u8 idata block_infor3;
check_loop:
    if(DEF_CHP_EN & 0x01)   { if(sata.CHP0_status_high == 0xff) {chp_sel = 0x01;sata.CHP_stripe = 0x10;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x02)   { if(sata.CHP1_status_high == 0xff) {chp_sel = 0x02;sata.CHP_stripe = 0x20;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x04)   { if(sata.CHP2_status_high == 0xff) {chp_sel = 0x04;sata.CHP_stripe = 0x40;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x08)   { if(sata.CHP3_status_high == 0xff) {chp_sel = 0x08;sata.CHP_stripe = 0x80;sata.CHP_stripe_h = 0x00;goto handle;}}
    if(DEF_CHP_EN & 0x10)   { if(sata.CHP4_status_high == 0xff) {chp_sel = 0x10;sata.CHP_stripe = 0x00;sata.CHP_stripe_h = 0x10;goto handle;}}
    goto end_loop;
handle:
    reset_engine();
    SFR_CHP_en = chp_sel;
    stop_all_chp;
    wait_chp_ready();
    SFR_dma_base = 0x0000;
    SFR_dma_max_num_l = 0x01;
    SFR_dma_max_num_h = 0x00;
    sata.CHP_cmd = CHP_CMD_CARD_CRC;
    SFR_dma_cntl = SFR_dma_cntl_run | SFR_dma_cntl_dir_read;
    while(!dma_done);
    wait_chp_ready();
    clear_dma_run;
    //save block information
    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x0000;
    block_infor0 = SFR_ext_sram_data;
    block_infor1 = SFR_ext_sram_data;
    block_infor2 = SFR_ext_sram_data;
    block_infor3 = SFR_ext_sram_data;
    //open all chp
    //read smart information
    sata.CHP_stripe = DEF_CHP_STRIPE;
    sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
    chp_burst(CHP_CMD_READ,0x01,smart_attribute);
    //save smart information
    SFR_ext_sram_cntl = 0x10;
    switch(chp_sel)
    {   
        case 0x01:
        {
            SFR_ext_sram_addr = smart_card_crc_addr0;
            break;
        }
        case 0x02:
        {
            SFR_ext_sram_addr = smart_card_crc_addr1;
            break;
        }
        case 0x04:
        {
            SFR_ext_sram_addr = smart_card_crc_addr2;
            break;
        }
        case 0x08:
        {
            SFR_ext_sram_addr = smart_card_crc_addr3;
            break;
        }
        case 0x10:
        {
            SFR_ext_sram_addr = smart_card_crc_addr4;
            break;
        }
        default: goto end_loop;

    }
    SFR_ext_sram_data = block_infor0;
    SFR_ext_sram_data = block_infor1;
    SFR_ext_sram_data = block_infor2;
    SFR_ext_sram_data = block_infor3;
    SFR_ext_sram_cntl = 0x00;
    tmp = gen_check_sum();
	SFR_ext_sram_addr = 0x01ff;
	SFR_ext_sram_data = tmp;
	chp_burst(CHP_CMD_WRITE,0x01,smart_attribute);
    reset_engine();
    SFR_watch_dog_high = 0x00;
    goto check_loop;
end_loop:
    reset_engine();
    return; 
}


#endif  //end support_smart
