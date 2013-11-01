

#ifdef SUPPORT_SECURITY

u8 temp_id;
u8 temp_data0;
u8 temp_data1;




void security_update_id(u16 base_addr,u8 which,u8 zro)
{
    temp_id = read_code(identify_data_addr + base_addr);
    if(zro == 0x00) temp_id = (temp_id & (~(0x01 << which)));
    else temp_id = (temp_id | (0x01 << which));
    enable_code_program;
    write_code(identify_data_addr + base_addr,temp_id);
    disable_code_program;
}


void security_set_pwd()
{
    tx_fis_5f(status_drq, error_no, pm_pio_write, 0x01, 0x00, status_bsy);
    sata_burst(PIO_WRITE,0x01);
    if(sata_burst_abort)
    {
        g_security_abort = 1;
        reset_engine();
        #ifdef SUPPORT_SMART1
        updata_smart(smart_crc_num_addr,0x01);
        reset_engine();
        #endif
	    return;
    }

    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x0000;
    temp_data0 = SFR_ext_sram_data;
    temp_data1 = SFR_ext_sram_data;
    SFR_ext_sram_cntl = 0x00;
    if((temp_data0 & 0x01) == 0x00)  //set user password
    {
        //security_state_flag = SEC5;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
        security_update_id(0x0100,0x01,0x01);    //word 128 bit 1 set to 1
        security_update_id(0x00aa,0x01,0x01);    //word 85 bit 1 set to 1
        if((temp_data1 & 0x01) == 0x00) security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
        else security_update_id(0x0101,0x00,0x01);    //word 128 bit 8 set to 1

        chp_burst(CHP_CMD_READ,0x01,security_user_log);
        SFR_ext_sram_cntl = 0x00;
        SFR_ext_sram_addr = 0x0050;
        SFR_ext_sram_data = 0x01;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
    }
    else                            //set master password
    {
        //if(g_security_enable == 0) security_state_flag = SEC1;
        //else security_state_flag = SEC5;
        chp_burst(CHP_CMD_WRITE,0x01,security_master_log);
        SFR_ext_sram_cntl = 0x10;
        SFR_ext_sram_addr = 0x0022;     //data word 17 
        temp_data0 = SFR_ext_sram_data;
        temp_data1 = SFR_ext_sram_data;
        if(((temp_data0 != 0x00) || (temp_data1 != 0x00)) && ((temp_data0 != 0xff) || (temp_data1 != 0xff)))
        {
            enable_code_program;
            write_code(identify_data_addr + 0x00b8,temp_data0); //word 92
            write_code(identify_data_addr + 0x00b9,temp_data1); //word 92
            disable_code_program;
        }
        
        chp_burst(CHP_CMD_READ,0x01,security_user_log);
        SFR_ext_sram_cntl = 0x00;
        SFR_ext_sram_addr = 0x0050;
        SFR_ext_sram_data = 0x01;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
    }
}


void security_unlock()
{
    tx_fis_5f(status_drq, error_no, pm_pio_write, 0x01, 0x00, status_bsy);
    sata_burst(PIO_WRITE,0x01);
    //security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
    
    if(sata_burst_abort)
    {
        g_security_abort = 1;
        #ifdef SUPPORT_SMART1
        updata_smart(smart_crc_num_addr,0x01);
        reset_engine();
        #endif
        return;
    }
    //SFR_ext_sram_cntl = 0x00;
    //SFR_ext_sram_addr = 0x0000;
    //temp_data0 = SFR_ext_sram_data & 0x01;
    check_pwd(); 
    if(g_security_pass == 0)
    {
        g_security_abort = 1;
        return;
    }
    else 
    {
        security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
        chp_burst(CHP_CMD_READ,0x01,security_user_log);
        SFR_ext_sram_cntl = 0x00;
        SFR_ext_sram_addr = 0x0060;
        SFR_ext_sram_data = security_mode;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
    }
    return;
}

void check_pwd()
{
    //g_security_pass = 1;
    u8 temp_data2;
    u8 temp_data3;
    u8 temp_data4;
    u8 temp_data5;
    u8 temp_data6;
    u8 temp_data7;
    u8 temp_data8;
    u8 temp_data9;
    u8 temp_data10;
    u8 temp_data11;
    u8 temp_data12;
    u8 temp_data13;
    u8 temp_data14;
    u8 temp_data15;
    u8 temp_data16;
    u8 temp_data17;
    u8 temp_data18;
    u8 temp_data19;
    u8 temp_data20;
    u8 temp_data21;
    u8 temp_data22;
    u8 temp_data23;
    u8 temp_data24;
    u8 temp_data25;
    u8 temp_data26;
    u8 temp_data27;
    u8 temp_data28;
    u8 temp_data29;
    u8 temp_data30;
    u8 temp_data31;

    g_security_pass = 0;
    SFR_ext_sram_addr = 0x0002;
    SFR_ext_sram_cntl = 0x10;
    temp_data0 = SFR_ext_sram_data;
    temp_data1 = SFR_ext_sram_data;
    temp_data2 = SFR_ext_sram_data;
    temp_data3 = SFR_ext_sram_data;
    temp_data4 = SFR_ext_sram_data;
    temp_data5 = SFR_ext_sram_data;
    temp_data6 = SFR_ext_sram_data;
    temp_data7 = SFR_ext_sram_data;
    temp_data8 = SFR_ext_sram_data;
    temp_data9 = SFR_ext_sram_data;
    temp_data10 = SFR_ext_sram_data;
    temp_data11 = SFR_ext_sram_data;
    temp_data12 = SFR_ext_sram_data;
    temp_data13 = SFR_ext_sram_data;
    temp_data14 = SFR_ext_sram_data;
    temp_data15 = SFR_ext_sram_data;
    temp_data16 = SFR_ext_sram_data;
    temp_data17 = SFR_ext_sram_data;
    temp_data18 = SFR_ext_sram_data;
    temp_data19 = SFR_ext_sram_data;
    temp_data20 = SFR_ext_sram_data;
    temp_data21 = SFR_ext_sram_data;
    temp_data22 = SFR_ext_sram_data;
    temp_data23 = SFR_ext_sram_data;
    temp_data24 = SFR_ext_sram_data;
    temp_data25 = SFR_ext_sram_data;
    temp_data26 = SFR_ext_sram_data;
    temp_data27 = SFR_ext_sram_data;
    temp_data28 = SFR_ext_sram_data;
    temp_data29 = SFR_ext_sram_data;
    temp_data30 = SFR_ext_sram_data;
    temp_data31 = SFR_ext_sram_data;
    SFR_ext_sram_cntl = 0x00;
    //////////////////////////////////////////
    //read user pwd
    security_mode = show_user;
    chp_burst(CHP_CMD_READ,0x01,security_user_log);
    
    //compare user pwd
    SFR_ext_sram_addr = 0x0002;
    SFR_ext_sram_cntl = 0x10;

    if(temp_data0 == SFR_ext_sram_data)
    if(temp_data1 == SFR_ext_sram_data)
    if(temp_data2 == SFR_ext_sram_data)
    if(temp_data3 == SFR_ext_sram_data)
    if(temp_data4 == SFR_ext_sram_data)
    if(temp_data5 == SFR_ext_sram_data)
    if(temp_data6 == SFR_ext_sram_data)
    if(temp_data7 == SFR_ext_sram_data)
    if(temp_data8 == SFR_ext_sram_data)
    if(temp_data9 == SFR_ext_sram_data)
    if(temp_data10 == SFR_ext_sram_data)
    if(temp_data11 == SFR_ext_sram_data)
    if(temp_data12 == SFR_ext_sram_data)
    if(temp_data13 == SFR_ext_sram_data)
    if(temp_data14 == SFR_ext_sram_data)
    if(temp_data15 == SFR_ext_sram_data)
    if(temp_data16 == SFR_ext_sram_data)
    if(temp_data17 == SFR_ext_sram_data)
    if(temp_data18 == SFR_ext_sram_data)
    if(temp_data19 == SFR_ext_sram_data)
    if(temp_data20 == SFR_ext_sram_data)
    if(temp_data21 == SFR_ext_sram_data)
    if(temp_data22 == SFR_ext_sram_data)
    if(temp_data23 == SFR_ext_sram_data)
    if(temp_data24 == SFR_ext_sram_data)
    if(temp_data25 == SFR_ext_sram_data)
    if(temp_data26 == SFR_ext_sram_data)
    if(temp_data27 == SFR_ext_sram_data)
    if(temp_data28 == SFR_ext_sram_data)
    if(temp_data29 == SFR_ext_sram_data)
    if(temp_data30 == SFR_ext_sram_data)
    if(temp_data31 == SFR_ext_sram_data)
    g_security_pass = 1;
    if(g_security_pass == 1)goto check_over;

    //read master pwd
    security_mode = show_master;
    chp_burst(CHP_CMD_READ,0x01,security_master_log);
    
    //compare user pwd
    SFR_ext_sram_addr = 0x02;
    SFR_ext_sram_cntl = 0x10;

    if(temp_data0 == SFR_ext_sram_data)
    if(temp_data1 == SFR_ext_sram_data)
    if(temp_data2 == SFR_ext_sram_data)
    if(temp_data3 == SFR_ext_sram_data)
    if(temp_data4 == SFR_ext_sram_data)
    if(temp_data5 == SFR_ext_sram_data)
    if(temp_data6 == SFR_ext_sram_data)
    if(temp_data7 == SFR_ext_sram_data)
    if(temp_data8 == SFR_ext_sram_data)
    if(temp_data9 == SFR_ext_sram_data)
    if(temp_data10 == SFR_ext_sram_data)
    if(temp_data11 == SFR_ext_sram_data)
    if(temp_data12 == SFR_ext_sram_data)
    if(temp_data13 == SFR_ext_sram_data)
    if(temp_data14 == SFR_ext_sram_data)
    if(temp_data15 == SFR_ext_sram_data)
    if(temp_data16 == SFR_ext_sram_data)
    if(temp_data17 == SFR_ext_sram_data)
    if(temp_data18 == SFR_ext_sram_data)
    if(temp_data19 == SFR_ext_sram_data)
    if(temp_data20 == SFR_ext_sram_data)
    if(temp_data21 == SFR_ext_sram_data)
    if(temp_data22 == SFR_ext_sram_data)
    if(temp_data23 == SFR_ext_sram_data)
    if(temp_data24 == SFR_ext_sram_data)
    if(temp_data25 == SFR_ext_sram_data)
    if(temp_data26 == SFR_ext_sram_data)
    if(temp_data27 == SFR_ext_sram_data)
    if(temp_data28 == SFR_ext_sram_data)
    if(temp_data29 == SFR_ext_sram_data)
    if(temp_data30 == SFR_ext_sram_data)
    if(temp_data31 == SFR_ext_sram_data)
    g_security_pass = 1;
    goto check_over;

    check_over:
    SFR_ext_sram_cntl = 0x00;
    return;
}


void security_initial()
{
    chp_burst(CHP_CMD_READ,0x01,security_user_log);
    SFR_ext_sram_cntl = 0x00;
    SFR_ext_sram_addr = 0x0060;
    security_mode = SFR_ext_sram_data;

    //enable or disable
    SFR_ext_sram_addr = 0x0050;
    if(SFR_ext_sram_data == 0x01) 
    {
    //sata.led0_blink = 0xf0;
    //while(1);
        //g_security_enable = 1;
        security_update_id(0x0100,0x01,0x01);    //word 128 bit 1 set to 1
        //security_state_flag = SEC4; //security enable
    }
    else 
    {
        //g_security_enable = 0;
        //security_state_flag = SEC1; //security disable
    }
    //analyze_states();
    return;
}

void analyze_states()
{
    switch(security_state_flag)
    {
        case SEC1:
        {
            g_security_enable = 0;
            g_security_lock = 0;
            g_security_frozen = 0;
            break;
        }
        
        case SEC2:
        {
            g_security_enable = 0;
            g_security_lock = 0;
            g_security_frozen = 1;
            break;
        }
        
        case SEC4:
        {
            g_security_enable = 1;
            g_security_lock = 1;
            g_security_frozen = 0;
            break;
        }
        
        case SEC5:
        {
            g_security_enable = 1;
            g_security_lock = 0;
            g_security_frozen = 0;
            break;
        }
        
        case SEC6:
        {
            g_security_enable = 1;
            g_security_lock = 0;
            g_security_frozen = 1;
            break;
        }
        default: break;
    }
}
/*
void mbr_burst(u8 cmd)//this is for samrt only 
{
	reset_engine();
	sata.SUP_LBA0 = 0x00;
	sata.SUP_LBA1 = 0x00;
	sata.SUP_LBA2 = 0x00;
	sata.SUP_LBA3 = 0x00;
	sata.SUP_LBA4 = 0x00;
	sata.SUP_LBA5 = 0x00;
	
	sata.CHP_cmd = cmd;
	SFR_dma_base = 0x0000;
    SFR_dma_max_num_l = 0x01;
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
}

void unlock_partion()
{
    u8 ii;
    
    mbr_burst(CHP_CMD_READ);
    delay(10);

    SFR_ext_sram_cntl = 0x10;
    SFR_ext_sram_addr = 0x01BE + 16 + 8;
    g_cap[0] = SFR_ext_sram_data;    //preceding capacity reading
    g_cap[1] = SFR_ext_sram_data;
    g_cap[2] = SFR_ext_sram_data;
    g_cap[3] = SFR_ext_sram_data;
    SFR_ext_sram_cntl = 0x00;

    SFR_ext_sram_addr = 0x01BE+16;
    SFR_ext_sram_cntl = 0x10;
    for(ii=0;ii<16*(4-1);ii++)
    {
        SFR_ext_sram_data = 0x00;
    }
    SFR_ext_sram_cntl = 0x00;
    chp_burst(CHP_CMD_WRITE,0x01,mbr_encryt_addr);
    //mbr_burst(CHP_CMD_WRITE);
    flush_delay();
}
*/

void choose_lba()
{
    u8 temp_lba;
    if(security_mode == show_master) 
    {
        enable_fetch_lba;
        temp_lba = sata.SUP_LBA3;
        disable_fetch_lba;
        sata.SUP_LBA3 = temp_lba + 0x03;
		
#ifdef security_debug		
		enable_fetch_lba;
	myprintf("csLBA:%x", sata.SUP_LBA3);
		disable_fetch_lba;
#endif
    }
    return;
}



#endif

