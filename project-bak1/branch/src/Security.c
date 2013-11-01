
u8 GPIO_old_value;

#ifdef SUPPORT_SECURITY

u8 idata temp_id;
u8 idata temp_data0;
u8 idata temp_data1;




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
	myprintf("\nsss abort");
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
    if((temp_data0 & 0x01) == 0x01)  //set master password
    {
        if(g_security_enable == 0) security_state_flag = SEC1;
        else security_state_flag = SEC5;
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

            chp_burst(CHP_CMD_READ,0x01,security_master_log);
            SFR_ext_sram_cntl = 0x00;
            SFR_ext_sram_addr = 0x0050;
            SFR_ext_sram_data = 0x01;
            chp_burst(CHP_CMD_WRITE,0x01,security_master_log);
        }
        else 
	{
	 g_security_abort = 1;
	 myprintf("\nmaster abort");
	}
    }
  else             //set user password
    {
        security_state_flag = SEC5;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
        if((temp_data1 & 0x01) == 0x00) 
        {
            g_security_HrM = 0;
        }
        else 
        {
            g_security_HrM = 1;
        }
        chp_burst(CHP_CMD_READ,0x01,security_user_log);
        SFR_ext_sram_cntl = 0x00;
        SFR_ext_sram_addr = 0x0050;
        SFR_ext_sram_data = 0x01;
        SFR_ext_sram_addr = 0x0060;
        if(g_security_HrM == 0) SFR_ext_sram_data = 0x00;
        else SFR_ext_sram_data = 0x10;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
    }                           
    
}


void security_dis_pwd()
{
    tx_fis_5f(status_drq, error_no, pm_pio_write, 0x01, 0x00, status_bsy);
    sata_burst(PIO_WRITE,0x01);
    if(sata_burst_abort)
    {
        g_security_abort = 1;
        #ifdef SUPPORT_SMART1
        updata_smart(smart_crc_num_addr,0x01);
        reset_engine();
        #endif
	    return;
    }
    
    SFR_ext_sram_cntl = 0x00;
    SFR_ext_sram_addr = 0x0000;
    temp_data0 = SFR_ext_sram_data & 0x01;
    if(g_security_enable == 0)  //security is disable
    {
        if(temp_data0 == user_pwd)
        {
            g_security_abort = 1;
            return;
        }
        else    //compare master pwd
        {
            check_pwd(); 
            if(g_security_pass == 0)
            {
                g_security_abort = 1;
                return;
            }
            else    //the password is right
            {
                //security_state_flag = SEC1;
            }
        }
    }
    else    //security is enable
    {
        if(g_security_HrM == 0) //Master Password Capability bit is "0": High
        {
            check_pwd();  //"0" is user and "1" is master
            if(g_security_pass == 0)
            {
                g_security_abort = 1;
                return;
            }
            else    //the password is right
            {
                //security_state_flag = SEC1;
            }
        }
        else    //Master Password Capability bit is "1" : Maximum
        {
            if(temp_data0 == master_pwd)
            {
                g_security_abort = 1;
                return;
            }
            else    //compare user pwd
            {
                check_pwd();
                if(g_security_pass == 0)
                {
                    g_security_abort = 1;
                    return;
                }
                else    //the password is right
                {
                    //security_state_flag = SEC1;
                }
            }
        }
    }
    if(g_security_pass == 1)
    {
        security_update_id(0x00aa,0x01,0x00);    //word 85 bit 1 set to 0
        security_update_id(0x0100,0x01,0x00);    //word 128 bit 1 set to 0
        security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
        g_security_HrM = 0;
		
	chp_burst(CHP_CMD_READ,0x01,security_user_log);
    	SFR_ext_sram_cntl = 0x00;
    	SFR_ext_sram_addr = 0x0060;
	SFR_ext_sram_data = 0x00;
	SFR_ext_sram_addr = 0x0050;
	SFR_ext_sram_data = 0x00;	
	chp_burst(CHP_CMD_WRITE,0x01,security_user_log);
	}
    
    return;
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
     if((security_mode ==show_master) || (security_mode ==show_user) || (security_mode ==show_all))
	 	g_security_pass=1;
     else
    		check_pwd(); 
	 
    if(g_security_pass == 0)
    {
        g_security_abort = 1;
        return;
    }
    else 
    {
        security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
       /* chp_burst(CHP_CMD_READ,0x01,security_user_log);
        SFR_ext_sram_cntl = 0x00;
        SFR_ext_sram_addr = 0x0060;
        SFR_ext_sram_data = security_mode;
        chp_burst(CHP_CMD_WRITE,0x01,security_user_log);*/
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
    	{
 	g_security_pass = 1;

	if((security_mode==show_master) || (security_mode==show_user) ||(security_mode==show_all)) 
		;
	else
		{
		if(g_TJ_ZSD==0x1)
			security_mode=show_user;
		else
			security_mode = show_all;
	}
    	}
     myprintf("\nchkusr");
    if(g_security_pass == 1)goto check_over;

    //read master pwd
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
      {
 	g_security_pass = 1;

	if((security_mode==show_master) || (security_mode==show_user) ||(security_mode==show_all)) 
		;
	else
		{

	if(g_TJ_ZSD==0x1)
		security_mode=show_master;
	else
		security_erase = show_all;//ZSD' master password is for erase
	
	}
    	}
	 myprintf("\nchkmsr");
    goto check_over;

    check_over:
    SFR_ext_sram_cntl = 0x00;
    return;
}


void security_initial()
{
    chp_burst(CHP_CMD_READ,0x01,security_user_log);
    SFR_ext_sram_cntl = 0x00;
    //high or maximum
    SFR_ext_sram_addr = 0x0060;
    if(SFR_ext_sram_data == 0x10)
    {
        g_security_HrM = 1;
    }
    else
    {
        g_security_HrM = 0;
    }
    //enable or disable
    SFR_ext_sram_addr = 0x0050;
    /////////////////////////////////////
    #ifdef SPEC_SECURITY
    SFR_ext_sram_data = 0x01;
    #endif
    /////////////////////////////////////
    //移动盘的ATA密码是不开启的
     if(g_SYS_YD==0x00)
    {
	 security_state_flag = SEC1; //security disable
    }
    else if((security_mode ==show_master) || (security_mode ==show_user) || (security_mode ==show_all))
    {
	 security_state_flag = SEC5; //security unlock not frozen
    }
   else
    {
	    if((SFR_ext_sram_data == 0x01)) 
	    {
	        security_state_flag = SEC4; //security enable
	    }
	    else 
	    {
	        security_state_flag = SEC1; //security disable
	    }
     }
    analyze_states();
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
            
            security_update_id(0x00aa,0x01,0x00);    //word 85 bit 1 set to 0
            security_update_id(0x0100,0x01,0x00);    //word 128 bit 1 set to 0
            security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
            security_update_id(0x0100,0x03,0x00);    //word 128 bit 3 set to 0
            if(g_security_cnt_expired == 0) security_update_id(0x0100,0x04,0x00);    //word 128 bit 4 set to 0
            else security_update_id(0x0100,0x04,0x01);    //word 128 bit 4 set to 1
            security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
            break;
        }
        
        case SEC2:
        {
            g_security_enable = 0;
            g_security_lock = 0;
            g_security_frozen = 1;
            
            security_update_id(0x00aa,0x01,0x00);    //word 85 bit 1 set to 0
            security_update_id(0x0100,0x01,0x00);    //word 128 bit 1 set to 0
            security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
            security_update_id(0x0100,0x03,0x01);    //word 128 bit 3 set to 1
            if(g_security_cnt_expired == 0) security_update_id(0x0100,0x04,0x00);    //word 128 bit 4 set to 0
            else security_update_id(0x0100,0x04,0x01);    //word 128 bit 4 set to 1
            if(g_security_HrM == 0) security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
            else security_update_id(0x0101,0x00,0x01);    //word 128 bit 8 set to 1
            break;
        }
        
        case SEC4:
        {
            g_security_enable = 1;
            g_security_lock = 1;
            g_security_frozen = 0;

            security_update_id(0x00aa,0x01,0x01);    //word 85 bit 1 set to 1
            security_update_id(0x0100,0x01,0x01);    //word 128 bit 1 set to 1
            security_update_id(0x0100,0x02,0x01);    //word 128 bit 2 set to 1
            security_update_id(0x0100,0x03,0x00);    //word 128 bit 3 set to 0
            if(g_security_cnt_expired == 0) security_update_id(0x0100,0x04,0x00);    //word 128 bit 4 set to 0
            else security_update_id(0x0100,0x04,0x01);    //word 128 bit 4 set to 1
            if(g_security_HrM == 0) security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
            else security_update_id(0x0101,0x00,0x01);    //word 128 bit 8 set to 1
            break;
        }
        
        case SEC5:
        {
            g_security_enable = 1;
            g_security_lock = 0;
            g_security_frozen = 0;
            
            security_update_id(0x00aa,0x01,0x01);    //word 85 bit 1 set to 1
            security_update_id(0x0100,0x01,0x01);    //word 128 bit 1 set to 1
            security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
            security_update_id(0x0100,0x03,0x00);    //word 128 bit 3 set to 0
            if(g_security_cnt_expired == 0) security_update_id(0x0100,0x04,0x00);    //word 128 bit 4 set to 0
            else security_update_id(0x0100,0x04,0x01);    //word 128 bit 4 set to 1
            if(g_security_HrM == 0) security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
            else security_update_id(0x0101,0x00,0x01);    //word 128 bit 8 set to 1
            break;
        }
        
        case SEC6:
        {
            g_security_enable = 1;
            g_security_lock = 0;
            g_security_frozen = 1;

            security_update_id(0x00aa,0x01,0x01);    //word 85 bit 1 set to 1
            security_update_id(0x0100,0x01,0x01);    //word 128 bit 1 set to 1
            security_update_id(0x0100,0x02,0x00);    //word 128 bit 2 set to 0
            security_update_id(0x0100,0x03,0x00);    //word 128 bit 3 set to 1
            if(g_security_cnt_expired == 0) security_update_id(0x0100,0x04,0x00);    //word 128 bit 4 set to 0
            else security_update_id(0x0100,0x04,0x01);    //word 128 bit 4 set to 1
            if(g_security_HrM == 0) security_update_id(0x0101,0x00,0x00);    //word 128 bit 8 set to 0
            else security_update_id(0x0101,0x00,0x01);    //word 128 bit 8 set to 1
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

/* 问 题 单: 4     修改人:huawei,   时间:2013/10/28 
   修改原因: 实现双盘备份功调整容量为相同大小为70G/
   /* BEGIN: Added by huawei, 2013/10/28   问题单号:4 */
/*
       
      	  sata.SUP_LBA3 = temp_lba + 0x08;
		   sata.SUP_LBA2 = temp_lba2 + 0xc0;
*/
/* END:   Added by huawei, 2013/10/28 */
void choose_lba()
{
    u8 temp_lba,temp_lba2;
	 if(security_mode == show_master) 
     {   enable_fetch_lba;
        temp_lba = sata.SUP_LBA3;
		temp_lba2 = sata.SUP_LBA2;
        disable_fetch_lba;

      	  sata.SUP_LBA3 = temp_lba + 0x08;
		   sata.SUP_LBA2 = temp_lba2 + 0xc0;
	//else if(security_mode == show_user) 
	  //sata.SUP_LBA3 = temp_lba + 0x01;
	
#ifdef security_debug		
		enable_fetch_lba;
	myprintf("csLBA:%x", sata.SUP_LBA3);
		disable_fetch_lba;
#endif
 }    	
    return;
}

#ifdef GPIO_security

void GPIO_security_init(void)
{
   SFR_spi_set |= 0xE0;//pull up SPI pinxj
   delay_ms(1);
   g_TJ_ZSD=0x01;//0:ZSD 1:TJX
   g_SYS_YD=0x01;//系统盘(1),移动盘(0)
}

/*process command from GPIO, */
/*mode:1GPIO can change partition,0GPIO can only erase partition*/
/* 问 题 单: 2     修改人:weihuawei,   时间:2013/10/23 
   修改原因: 实现任意盘的销毁 */
   /* BEGIN: Added by weihuawei, 2013/10/23   问题单号:2 */
/*
#define GPIO_CMD_INACT        0x0F  
#define GC_TJ_erase_user	  0x01 
#define GC_TJ_erase_master    0x08 
#define GC_TJ_erase_all   0x0C 
                             case GC_TJ_erase_user:
					 security_erase=show_user;
					break;
				case GC_TJ_erase_master:
					 security_erase=show_master;
					break;
				case GC_TJ_erase_all:
					security_erase=show_all;
					break;
*/
/* END:   Added by weihuawei, 2013/10/23 */
void GPIO_security_cmd(u8 mode)
{
	u8 tmp_value;
	
	tmp_value=SFR_spi_io;
	if(GPIO_old_value == (tmp_value&0x0F))
		return;
	else
		GPIO_old_value= tmp_value;

	
	if((tmp_value & GPIO_CMD_INACT) == GPIO_CMD_INACT)//active low
		return;
	else
		{
		delay_ms(1);
		tmp_value=SFR_spi_io;
		if((tmp_value & GPIO_CMD_INACT) == GPIO_CMD_INACT)//skew
			return;
		else
			{
			tmp_value &=0x0F;
			myprintf("\ngpio=:%x",tmp_value);
			switch(tmp_value)
				{
				case GC_TJ_show_master:
					if((g_SYS_YD==0x01) && ((security_mode==show_master) || (security_mode==show_user) ||(security_mode==show_all))) return;
					if(/*(!g_security_enable) &&*/ (mode==0x1))//when using ata password,APP can not switch partitions
						{
						security_mode=show_master;
						//系统版切换分区时不能重启，如果系统没有ATA密码应该重启??
						if((!g_security_enable) || (g_SYS_YD==0x00))
							uart_show=0x1;
						}
					break;
					
				case GC_TJ_show_user:
					if((g_SYS_YD==0x01) && ((security_mode==show_master) || (security_mode==show_user) ||(security_mode==show_all))) return;
					if(/*(!g_security_enable) &&*/ (mode==0x1))//when using ata password,APP can not switch partitions
						{ 
						security_mode=show_user;
						//系统版切换分区时不能重启，如果系统没有ATA密码应该重启??
						if((!g_security_enable) || (g_SYS_YD==0x00))
							uart_show=0x1;
						}
					break;
					
				case GC_TJ_erase_user:
					 security_erase=show_user;
					break;						  
				case GC_TJ_erase_master:
					 security_erase=show_master;
					break;
				case GC_TJ_erase_all:
					myprintf("\n ERASE ALL 1");
					security_erase=show_all;
					break;
				case GC_ZSD_show_disk:
					if((g_SYS_YD==0x01) && ((security_mode==show_master) || (security_mode==show_user) ||(security_mode==show_all))) return;
					if(/*(!g_security_enable) &&*/ (mode==0x1))//when using ata password,APP can not switch partitions
						{
						security_mode=show_all;
						//系统版切换分区时不能重启，如果系统没有ATA密码应该重启??
						if((!g_security_enable) || (g_SYS_YD==0x00))
							uart_show=0x1;
						myprintf("\n master");
						}
					break;
					
				case GC_ZSD_erase_disk:
						security_erase=show_all;
						myprintf("\n ea");
					break;

				case GC_DISK_is_ZSD:	
					if((g_SYS_YD==0x01) && ((security_mode==show_master) || (security_mode==show_user) ||(security_mode==show_all))) return;
						if(/*(!g_security_enable) &&*/ (mode==0x1))//when using ata password,APP can not switch partitions
						{
						security_mode=show_all;
						//系统版切换分区时不能重启，如果系统没有ATA密码应该重启??
						if((!g_security_enable) || (g_SYS_YD==0x00))
							uart_show=0x1;
						myprintf("\n sa");
						}
					break;
					
				case GC_DISK_is_TJ:
					//security_cmd_info_write('Y', scl_security_nd_chk);
					break;
				default:
					
					break;

			}
		}
	}
}

#endif

/*to save some non-volatile security command information*/
void security_cmd_info_write(u8 value, u8 addr)
{
	SFR_ext_sram_cntl = 0x00;
	SFR_ext_sram_addr = addr;
	SFR_ext_sram_data = value;
	chp_burst(CHP_CMD_WRITE,0x01,security_cmd_log);
}

/*u8 security_cmd_info_read(u8 addr)
{
	u8 value;
	
	SFR_ext_sram_cntl = 0x00;
	chp_burst(CHP_CMD_READ,0x01,security_cmd_log);
	SFR_ext_sram_addr = addr;
	value=SFR_ext_sram_data ;
	return value;
}*/

#endif

