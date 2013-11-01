#ifdef BUF_CMD_EN

u8 cmd_buffer_fetch_bus_sel()
{
    /*   
    #ifndef CHP_NUMBER
    #error "CHP_NUMBER not defined"

    #elif CHP_NUMBER == 1
    return DEF_CHP_EN;

    #elif CHP_NUMBER == 2
    if((sata.SUP_LBA0 & 0x10) == 0x00)return 0x01;
    else return 0x02;

    #elif CHP_NUMBER == 4
    u8 tmp_data;
    //NOTE: only implement stripe size == 8K here
    tmp_data = sata.SUP_LBA0;
    if((tmp_data& 0x30)==0x00)return 0x01;
    if((tmp_data& 0x30)==0x10)return 0x02;
    if((tmp_data& 0x30)==0x20)return 0x04;
    if((tmp_data& 0x30)==0x30)return 0x08;

    #elif CHP_NUMBER == 5
    return sata.chp_bus_sel;

    #else
    #error: "CHP_NUMBER is defined, but invalid."
    #endif
    */
    BYTE stripe_sel;
    
    
    if(CHP_NUMBER==0x01)return 0x01;

    if(CHP_NUMBER==0x02)
    {
    	
       if(STRIPE_8K_SET)stripe_sel = 0x10; //8k
       else if(STRIPE_4K_SET)stripe_sel = 0x08; //4k
       else stripe_sel = 0x04; //2K
	   
       if((sata.SUP_LBA0 & stripe_sel) == 0x00)return 0x01;
       else return 0x02; 
    }
    if(CHP_NUMBER==0x04)
    {
        u8 tmp_data;

        tmp_data = sata.SUP_LBA0;
		if(STRIPE_8K_SET)//8k
		{
		    stripe_sel = 0x30;
			if((tmp_data& stripe_sel)==0x00)return 0x01;
        	if((tmp_data& stripe_sel)==0x10)return 0x02;
        	if((tmp_data& stripe_sel)==0x20)return 0x04;
        	if((tmp_data& stripe_sel)==0x30)return 0x08;
		}
		else if(STRIPE_4K_SET)//4k
		{
		    stripe_sel = 0x18;
        	if((tmp_data& stripe_sel)==0x00)return 0x01;
        	if((tmp_data& stripe_sel)==0x08)return 0x02;
        	if((tmp_data& stripe_sel)==0x10)return 0x04;
        	if((tmp_data& stripe_sel)==0x18)return 0x08;
		}
		else //2K
		{
		    stripe_sel = 0x0C;
        	if((tmp_data& stripe_sel)==0x00)return 0x01;
        	if((tmp_data& stripe_sel)==0x04)return 0x02;
        	if((tmp_data& stripe_sel)==0x08)return 0x04;
        	if((tmp_data& stripe_sel)==0x0C)return 0x08;
		}
     }

    //if(CHP_NUMBER==0x05)
    {
        return sata.chp_bus_sel;
    }

}



void cmd_buffer_set_param()
{
    save_lba();
    stop_all_chp;
    
    buf_data_pending = 1;
    //all pkt_cell are 4k
    /*
    if(g_sec_cnt.byte.h != 0x00)goto normal_set;
    if(g_sec_cnt.byte.l==0x01) //512 => 4k
    {
        buf_pkt_cell_next = 0x08;
        buf_pkt_depth_next = 0x08;
        return;
    }
    if(g_sec_cnt.byte.l==0x02)//1k => 4k
    {
        buf_pkt_cell_next = 0x08;
        buf_pkt_depth_next = 0x08;
        return;
    }
    if(g_sec_cnt.byte.l==0x04)//2k => 4k
    {
        buf_pkt_cell_next = 0x08;
        buf_pkt_depth_next = 0x08;
        return;
    }
    if(g_sec_cnt.byte.l==0x08)//4k => 4k
    {
        buf_pkt_cell_next = 0x08;
        buf_pkt_depth_next = 0x08;
        return;
    }
    */
//normal_set:                   //other => 4k
    buf_pkt_cell_next = 0x08;
    buf_pkt_depth_next = 0x08;

   
     
    return;
}

void cmd_buffer_init()
{
	buf_flag = 0x00;
	buf_cmd_flag= 0x00;
	buf_check_cnt = 0x00;
	enable_code_program;
    g_tmp_addr = BUF_TABLE_BASE;
    for(i=0;i<BUF_MAX_DEPTH;i++)
    {
        write_code(g_tmp_addr,BUF_SEL_NONE);
        g_tmp_addr = g_tmp_addr + BUF_LINE_LEN;
    }
    disable_code_program;
    for(i=0;i<MAX_CHP_NUMBER;i++)
    {
        buf_wr_index[i] = 0x00;
        buf_rd_index[i] = 0x00;
    }
    buf_rd_ptr = 0x00;
    buf_wr_ptr = 0x00;
    buf_pkt_cell = 0x08;
    buf_pkt_depth = 0x08;
    
}


//only ONE PKT_CELL for each call
void cmd_buffer_write()
{
    u8 tmp_chp_id, tmp_chp_sel;
    //we may recover from abnormal state
    clear_dma_run;
    
   /* sata.SUP_LBA0 = g_lba[0];
    sata.SUP_LBA1 = g_lba[1];
    sata.SUP_LBA2 = g_lba[2];
    sata.SUP_LBA3 = g_lba[3];
    sata.SUP_LBA4 = g_lba[4];
    sata.SUP_LBA5 = g_lba[5];*/
    
    
    g_tmp_addr = (BUF_TABLE_BASE + ADDR_BUF_SEL);
    SFR_sata_burst_base_l = 0x00;
    SFR_sata_burst_base_h = 0x00;

    //search for a empty location: BUF_SEL_NONE.
    //certainly we will find one, otherwise this is an error!!!
    for(i=0;i<buf_pkt_depth;i++)
    {
        if(read_code(g_tmp_addr)==BUF_SEL_NONE)
        {
            //I don't want to use i as my local variable
            break;
        }
        g_tmp_addr += BUF_LINE_LEN;
        SFR_sata_burst_base_h += 2*buf_pkt_cell;
    }
    
    buf_data_ok = 1;

    //init BUF_NUM
    g_tmp[ADDR_BUF_NUM] = 0;
    
    //init CHP bus sel
    g_tmp[ADDR_BUF_SEL] = cmd_buffer_fetch_bus_sel();
    
    //just change the chp_sel to chp_id
    tmp_chp_id = 0x01;
    for(i=0;i<MAX_CHP_NUMBER;i++)
    {
        if(tmp_chp_id == g_tmp[ADDR_BUF_SEL])
        {
            tmp_chp_id = i;
            break;
        } 
        tmp_chp_id = tmp_chp_id << 1;
    }
    //(1)NCQ, we have set the LBA to SUP_LBA in ncq_handler()
    //(2)non-NCQ, we have set the LBA in check_lba();
    enable_fetch_lba;
    g_tmp[ADDR_BUF_LBA0] = sata.SUP_LBA0;    
    g_tmp[ADDR_BUF_LBA1] = sata.SUP_LBA1;
    g_tmp[ADDR_BUF_LBA2] = sata.SUP_LBA2;    
    g_tmp[ADDR_BUF_LBA3] = sata.SUP_LBA3;
    g_tmp[ADDR_BUF_LBA4] = sata.SUP_LBA4;
    g_tmp[ADDR_BUF_LBA5] = sata.SUP_LBA5;   
    disable_fetch_lba;

    //always burst 512, and test the LBA
    SFR_sata_burst_cnt_l= 0x01;
    SFR_sata_burst_cnt_h= 0x00;
write_buffer_loop:
   
    //if buf_sector_num[3:0] is not zero, this is not 16 sectors boundary, need to block dma active
   // myprintf("\nW:N=%x",buf_sector_num.byte.l);
    
	if((sata.ncq_cntl & ncq_cntl_link_idle) != ncq_cntl_link_idle)sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_block_active;

    //set running to burst data into FIFO
    SFR_sata_burst_cntl = SFR_sata_burst_cntl_run | SFR_sata_burst_cntl_dma_act;
    while(!sata_burst_done) 
    {
        if(g_current_fis_num != SFR_fis_num)
        {
            buf_new_cmd = 1;
            SFR_sata_burst_cntl = 0x00;
            sata.ncq_cntl = sata.ncq_cntl & (~ncq_cntl_block_active);
            return;
        }
    }
    sata.ncq_cntl = sata.ncq_cntl & (~ncq_cntl_block_active);
    if(sata_burst_abort)
    {
        SFR_sata_burst_cntl = 0x00;
        buf_data_ok = 0; 
        return;
    }
     
    buf_sector_num.word++;
    g_tmp[ADDR_BUF_NUM]+=0x01;
    SFR_sata_burst_cntl = 0x00;   
    SFR_sata_burst_base_h += 0x02;
    auto_increase_lba;

   
    //if switch to another CHP
    tmp_chp_sel = cmd_buffer_fetch_bus_sel();
    

    //over: if all done, switch bus, pkt_cell full
    if(g_tmp[ADDR_BUF_SEL] != tmp_chp_sel) goto write_buffer_over;
    if(g_tmp[ADDR_BUF_NUM] == buf_pkt_cell) goto write_buffer_over;
    if(buf_sector_num.word == g_sec_cnt.word)goto write_buffer_over;
    goto write_buffer_loop;
    

write_buffer_over:

    g_tmp[ADDR_BUF_INDEX] = buf_wr_index[tmp_chp_id];

    //set whether this is a new command. 
    //If received a new continue commmand, buf_cmd_flag will be zero. 
    //Which will set this is NOT a new command
    if(g_tmp[ADDR_BUF_SEL] & buf_cmd_flag)g_tmp[ADDR_BUF_NEW_CMD] = 0x01;
    else g_tmp[ADDR_BUF_NEW_CMD] = 0x00;
    
    
    enable_code_program;
    for(i=0;i<BUF_LINE_LEN;i++)
    {
        write_code(g_tmp_addr,g_tmp[i]);
        g_tmp_addr++;
    }
    disable_code_program;

    //Set this is not a new commmand for the next loop.(buf_cmd_flag will update if receiving a new command)
    buf_cmd_flag = buf_cmd_flag & (~g_tmp[ADDR_BUF_SEL]);
    buf_wr_index[tmp_chp_id] += 0x01;
    buf_wr_ptr++;
    
    // save_lba();
    
    
}



void cmd_buffer_read()
{

    u8 tmp_chp_id, tmp_chp_sel;

    tmp_chp_id = 0x00;
    tmp_chp_sel = CHP_MIN_SEL;
    SFR_dma_base_l= 0x00;
    SFR_dma_max_num_h = 0x00;
    
read_buffer_loop:
    SFR_dma_base_h= 0x00;
    g_tmp_addr = BUF_TABLE_BASE + ADDR_BUF_SEL;
    //this search SHOULD BE optimized.
    i = 0;
    while(i<buf_pkt_depth)
    {
    	if((read_code(g_tmp_addr)==tmp_chp_sel) && (read_code(g_tmp_addr+ADDR_BUF_INDEX-ADDR_BUF_SEL)== buf_rd_index[tmp_chp_id]))
        {
			goto find_cell;
        }
        g_tmp_addr += BUF_LINE_LEN;
        SFR_dma_base_h +=  2*buf_pkt_cell;
        i++;
    }
    //means not found
    //if(tmp_buf_id == 0xff) 
	goto end_one_loop;
    find_cell:
    for(i=0;i<BUF_LINE_LEN;i++)
    {
        g_tmp[i] = read_code(g_tmp_addr);
        g_tmp_addr++;   
    }

    //load the LBA. we may refine the following six line code
    sata.SUP_LBA0 = g_tmp[ADDR_BUF_LBA0];
    sata.SUP_LBA1 = g_tmp[ADDR_BUF_LBA1];
    sata.SUP_LBA2 = g_tmp[ADDR_BUF_LBA2];
    sata.SUP_LBA3 = g_tmp[ADDR_BUF_LBA3];
    sata.SUP_LBA4 = g_tmp[ADDR_BUF_LBA4];
    sata.SUP_LBA5 = g_tmp[ADDR_BUF_LBA5];
    //check command is continue, if continue, don't need to send stop or check ready
    
    if (g_tmp[ADDR_BUF_NEW_CMD]==0x00)goto burst_to_chp;
    
    //start to check the specific CHP
    SFR_CHP_en = tmp_chp_sel;
    sata.CHP_cmd =CHP_CMD_STOP;
    for(i=0;i<1;i++)
    {
        delay(10);
        if(chp_is_ready)goto data_start;
    }
    goto end_one_loop;
data_start: 

	enable_fetch_lba;
	myprintf("\nBUFr0: %x %x %x %x %x %x ",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
	disable_fetch_lba;	
	
   // myprintf("\nchp0_status:%x",sata.CHP0_status);
    sata.CHP_cmd = CHP_CMD_WRITE;
    //myprintf("\nR:=>%x",tmp_chp_id);
 burst_to_chp:
    //start to burst data to CHP
    SFR_dma_max_num_l = g_tmp[ADDR_BUF_NUM];
   // myprintf("\ndma_max_num:%x%x",SFR_dma_max_num_h,SFR_dma_max_num_l);
   // myprintf("\nlba:%x%x%x%x%x%x",sata.SUP_LBA5,sata.SUP_LBA4,sata.SUP_LBA3,sata.SUP_LBA2,sata.SUP_LBA1,sata.SUP_LBA0);
    SFR_dma_cntl = SFR_dma_cntl_run;
    while(!dma_done);
    SFR_dma_cntl = 0x00;    
    buf_rd_index[tmp_chp_id] += 0x01;
    buf_rd_ptr++;
    //update the select
    g_tmp_addr -= BUF_LINE_LEN;
    enable_code_program;
    write_code(g_tmp_addr+ADDR_BUF_SEL,BUF_SEL_NONE);
    disable_code_program;


    //todo: decide to goto read_buffer_over or start another loop.
   // goto read_buffer_over;
   goto read_buffer_loop;
end_one_loop:
    if(tmp_chp_sel==CHP_MAX_SEL)goto read_buffer_over;
    tmp_chp_id++;
    tmp_chp_sel = tmp_chp_sel << 1;
    goto read_buffer_loop;
    
read_buffer_over:
    //NOTE: always keep this register back to DEF_CHP_EN in every sub-routine!!!
    SFR_CHP_en = DEF_CHP_EN;
    return;
    
}

void cmd_buffer_check()
{
    
    //(1)if there is data, try burst it to CHP
    if(buf_not_empty)
    {
   //    myprintf("\nR:O,w:%x, r:%x",buf_wr_ptr,buf_rd_ptr);
       
       //when phy is down, we will fluch buffer: call cmd_buffer_read().
       //we don't want to call cmd_buffer_read() in sub routine cmd_buffer_read().
       sata.int2_en &= ~int2_en_phyrdy_n;
       cmd_buffer_read();
	   sata.int2_en |= int2_en_phyrdy_n;
	   
       SFR_watch_dog_high = 0;
    }
    //(2)if there is data pending, continue to accept data from SATA
    if(buf_data_pending)
    {
    //    myprintf("\nW:K,w:%x, r:%x",buf_wr_ptr,buf_rd_ptr);
        if(buf_is_full)return;
        //whenever switch pkt_cell or buf_cont_inside, wait buffer empty!!!
        if(buf_not_empty)
        {
            if(buf_pkt_cell!=buf_pkt_cell_next)return;
        }
        buf_pkt_cell = buf_pkt_cell_next;
        buf_pkt_depth = buf_pkt_depth_next;
        cmd_buffer_write();
        //(2-i)if receive a new command while not data after sending active.
        //usually buf_new_cmd==SRST
        if(buf_new_cmd) 
        {
            buf_new_cmd = 0;
            return;
        }
        //(2-a)if data fail, such as CRC error
        if(buf_data_ok==0)
        {
            tx_fis_34(status_bad,error_abort,int_set);
            buf_data_pending = 0;
            reset_engine();
            #ifdef SUPPORT_SMART1
            updata_smart(smart_crc_num_addr,0x01);
            reset_engine();
            #endif
        }
        //(2-b)if data all done
        //NOTE: for g_sec_cnt.word == 0x0000
        //
        else if( ((buf_cmd_flag & DEF_CHP_EN) != DEF_CHP_EN)  &&    //this indicates write have start, to avoid 0x0000 => 0x10000
            ( buf_sector_num.word == g_sec_cnt.word)
          )
        {
            tx_fis_34(status_good,error_no,int_set);
            buf_data_pending = 0;
        }
        SFR_watch_dog_high = 0;
    }
    return;

}





#endif


