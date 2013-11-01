#ifdef SUPPORT_NCQ

//(1)not handle this case: receive a new NCQ command when BSY is set.   
//    => we may choose to complete this later if necessary
void ncq_resp_cmd()
{

    /////////////////////////////////////////////////
    //(1)fetch TAG, Direction, LBA, Seccnt.
    /////////////////////////////////////////////////
    
    //ncq tag is in sectorcnt[7:3]
    g_tmp_tag= sata.fis_rcv_content[4*3+0];   //tag
    g_tmp_tag = g_tmp_tag >> 3;
    
    
    //fetch lba and sector cnt
    
    //g_tmp[0] is the tag status;
    g_tmp[1]          = g_flag_dir;
    g_tmp[2]          = sata.fis_rcv_content[4*0+3]; //sector count low
    g_tmp[3]          = sata.fis_rcv_content[4*1+0]; //lba0
    g_tmp[4]          = sata.fis_rcv_content[4*1+1];
    g_tmp[5]          = sata.fis_rcv_content[4*1+2];
    g_tmp[6]          = sata.fis_rcv_content[4*2+0];
    g_tmp[7]          = sata.fis_rcv_content[4*2+1];
    g_tmp[8]          = sata.fis_rcv_content[4*2+2]; //lba5
    g_tmp[9]          = sata.fis_rcv_content[4*2+3]; //sector count high
    g_tmp[10]        = sata.fis_rcv_content[4*1+3];//device
    g_tmp[11]        = sata.fis_rcv_content[4*3+2]; //ICC

    //LBA ERROR, simple handling.
    //todo: refine this or delete this
    if(SFR_lba_comp & lba_up_exceed)
    {
        tx_fis_34(status_bad,error_abort,int_set);
        g_ncq_halt = 1;
        return;
    }
    

    
    g_tmp_addr = g_tmp_tag * NCQ_LINE_LEN+ NCQ_TABLE_BASE;
    
    ////////////////////////////////////////////////////   
    //(2)check NCQ table: check whether received TAG is valid: not an used tag.
    //    If it is a pending(used) tag, send BAD SET_DEV_BITS FIS to host. (refer to spec. for detail) and then return
    ////////////////////////////////////////////////////
    
    if(read_code(g_tmp_addr) != TAG_FREE)
    {
              
       //assume this FIS A1 will always be sent successfully
       sata.quick_sactive = quick_sactive_clear; //clear to all zero
       tx_fis_a1(status_bad,error_abort,int_set);
       g_ncq_halt = 1;
       return;
    }

    /////////////////////////////////////////////////////
    //(3)If TAG valid, update the NCQ table 
    /////////////////////////////////////////////////////
    enable_code_program;
    write_code(g_tmp_addr,TAG_PENDING);
    for(i=1;i<NCQ_LINE_LEN;i++)
    {
        g_tmp_addr ++;
        write_code(g_tmp_addr,g_tmp[i]); 
    }
    disable_code_program;
    
    

    //////////////////////////////////////////////////////
    //(4)Send successful FIS34 to host and set the pending tag.
    //////////////////////////////////////////////////////
    
    //(4-1)Assume FIS 34 will always be sent.
    //todo:int no confirm
    tx_fis_34(status_good,error_no,int_no);
    
    
    return;
    
    
}




void tx_fis_a1(u8 status, u8 error, u8 flag)
{
    sata.FIS_error	 = error;
    sata.FIS_status    = status;
    if(flag!=0)  sata.FIS_pm |= 0x40 ;

    sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_block_rdy;
    sata.FIS_type	=  0xa1;  
    if(sata.ncq_cntl & ncq_cntl_tx_fis_abort)g_fis_tx_abort = 1;
    else g_fis_tx_abort = 0;
    sata.ncq_cntl = sata.ncq_cntl & (~ncq_cntl_block_rdy);
}



void tx_fis_41(u8 tag)
{
    g_tmp_addr = tag * NCQ_LINE_LEN+ NCQ_TABLE_BASE;
    //fetch NCQ table
    for(i=1;i<NCQ_LINE_LEN;i++)
    {
        g_tmp_addr ++;
        g_tmp[i] = read_code(g_tmp_addr);
    }
    
    //D_BIT and A_BIT
    g_flag_dir = g_tmp[ADDR_DIR];
    if(g_flag_dir==DIR_READ)sata.FIS_pm = 0x20; //D_BIT
    else sata.FIS_pm = 0x00;    //A_BIT
    
    //TAG
    sata.FIS_content_1_0 = tag;
    sata.FIS_content_1_1 = 0x00;
    sata.FIS_content_1_2 = 0x00;
    sata.FIS_content_1_3 = 0x00;

    sata.FIS_content_2_0 = 0x00;
    sata.FIS_content_2_1 = 0x00;
    sata.FIS_content_2_2 = 0x00;
    sata.FIS_content_2_3 = 0x00;
    
    sata.FIS_content_4_0 = 0x00;
    sata.FIS_content_4_1 = 0x00;
    sata.FIS_content_4_2 = 0x00;
    sata.FIS_content_4_3 = 0x00;

     
    //(a) sector => byte (x 0x200)
    //(b) check all 0 => 65536
    sata.FIS_content_5_0 = 0x00;
    sata.FIS_content_5_1 = (g_tmp[ADDR_SECCNT_LOW]  << 1);
    sata.FIS_content_5_2 = (g_tmp[ADDR_SECCNT_HIGH] << 1) + (g_tmp[ADDR_SECCNT_LOW]  >> 7);
    sata.FIS_content_5_3 = (g_tmp[ADDR_SECCNT_HIGH] >> 7);
    if((g_tmp[ADDR_SECCNT_LOW] == 0x00) && (g_tmp[ADDR_SECCNT_HIGH] == 0x00))sata.FIS_content_5_3 = 0x02;

    sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_block_rdy;
    sata.FIS_type	=  0x41;  
    if(sata.ncq_cntl & ncq_cntl_tx_fis_abort)g_fis_tx_abort = 1;
    else g_fis_tx_abort = 0;
    sata.ncq_cntl = sata.ncq_cntl & (~ncq_cntl_block_rdy);
}



//send dma set up FIS to request data transfer
void ncq_handler()
{
    /////////////////////////////////////////////////
    //(1)fetch TAG: tag from hardware or from CHPs
    /////////////////////////////////////////////////

    //(1-a)check whether we can continue
    //todo later:
    g_ncq_cont = 0;
    
    //(1-b)check whether we can request more efficient command first
    //need to know which CHP this will command will distribute to.
    //need to check each CHP status.

    
    //otherwise firmware search for a tag whose status is PENDING
    g_tmp_addr = NCQ_TABLE_BASE;
    for(i=0;i<NCQ_MAX_TAG;i++)
    {
        if(read_code(g_tmp_addr) == TAG_PENDING)
        {
            g_tmp_tag = i;
            break;
        }
        g_tmp_addr = g_tmp_addr + NCQ_LINE_LEN;
    }
    //if not pending tag, just return;
    if(i==NCQ_MAX_TAG)
    {
        ncq_set_dev_bits();
        return;
    }

    //open CHP clock and clear the watch dog
    chp_clock_on();

   
    
    
    
    /////////////////////////////////////////////////
    //(2)send DMA_SETUP FIS, if fail return
    /////////////////////////////////////////////////
    tx_fis_41(g_tmp_tag);
    if(g_fis_tx_abort)return;
    

    
    
    /////////////////////////////////////////////////
    //(3)send DMA_SETUP FIS successfully, start data transfer.
    /////////////////////////////////////////////////

   

    //(3-b)load lba and sector count
    sata.SUP_LBA0 = g_tmp[ADDR_LBA0];
    sata.SUP_LBA1 = g_tmp[ADDR_LBA1];
    sata.SUP_LBA2 = g_tmp[ADDR_LBA2];
    sata.SUP_LBA3 = g_tmp[ADDR_LBA3];
    sata.SUP_LBA4 = g_tmp[ADDR_LBA4];
    sata.SUP_LBA5 = g_tmp[ADDR_LBA5];
    g_sec_cnt.byte.l = g_tmp[ADDR_SECCNT_LOW];
    g_sec_cnt.byte.h = g_tmp[ADDR_SECCNT_HIGH];
    
    
    #ifdef BUF_CMD_EN
        if(g_flag_dir==DIR_WRITE)
        {
            enable_fetch_lba;
            g_lba[0] = g_tmp[ADDR_LBA0];
            g_lba[1] = g_tmp[ADDR_LBA1];
            g_lba[2] = g_tmp[ADDR_LBA2];
            g_lba[3] = g_tmp[ADDR_LBA3];
            g_lba[4] = g_tmp[ADDR_LBA4];
            g_lba[5] = g_tmp[ADDR_LBA5];
            disable_fetch_lba;
            SFR_dma_max_num_l = g_tmp[ADDR_SECCNT_LOW];
            SFR_dma_max_num_h = g_tmp[ADDR_SECCNT_HIGH];
            
            
            if(g_ncq_cont ||  (g_sec_cnt.byte.h!=0x00) || (g_sec_cnt.byte.l==0x00) || (g_sec_cnt.byte.l>BUF_SEC_CNT_THRESHOLD)) 
            {
                if(buf_not_empty)return;
                else
                {
                   goto ncq_data_start;
                }
            } 
            else
            {
                myprintf("\nNW");
                buf_sector_num.word = 0;
                buf_cmd_flag = DEF_CHP_EN;
                cmd_buffer_set_param();
                return; 
            } 
            
         }
         else //g_flag_dir == DIR_READ
         {
            //simple handle, we may optimize the search for TAG PENDING
            if(buf_not_empty)return;
         }
    #endif
#ifdef BUF_CMD_EN
ncq_data_start:
#endif
    ncq_data_handler();
    return;
}

void ncq_data_handler()
{
   
    //we still support continue for FPDMA
    if(g_ncq_cont)SFR_CHP_en= 0x00;
	
    stop_all_chp;
	wait_chp_ready();
	
    //load from NCQ TABLE, hardware will not automatically set bit[16] for us
    if(SFR_dma_max_num==0x0000)sata.ncq_cntl = sata.ncq_cntl | ncq_cntl_set_dma_cnt_high;
    
    //(1)set running
	if(g_flag_dir==DIR_WRITE)
    {
        sata.CHP_cmd = CHP_CMD_WRITE; 
        set_dma_run_wr;
    }           
	else
    {
        sata.CHP_cmd = CHP_CMD_READ; 
        set_dma_run_rd;
    }         
                
			
	//(2) wait done
	#ifdef ONLY_SUP
		//clear CHP run:
		//		(1)not to block sata burst(set wHold and rHold to zero.)
		//		(2)disable CHP burst		
		clear_chp_run;
		while(!sata_burst_done);
		if(sata_burst_abort)goto ncq_handler_err;
	#else
		while(!sata_burst_done);
		if(sata_burst_abort)goto ncq_handler_err;
		if(~g_flag_dir)while(!dma_done);	
	#endif

    //(3)after done, update NCQ table
    enable_code_program;
    g_tmp_addr = g_tmp_tag * NCQ_LINE_LEN+ NCQ_TABLE_BASE;
    write_code(g_tmp_addr,TAG_DONE);
    sata.quick_sactive = g_tmp_tag;
    disable_code_program;
    
    
    goto ncq_handler_over;
    /////////////////////////////////////////////////
    //(4)error during data transfer handling, then return
    /////////////////////////////////////////////////
ncq_handler_err:
    sata.quick_sactive = quick_sactive_clear;  //clear to all zero
    tx_fis_a1(status_bad,error_abort,int_set);
    g_ncq_halt = 1;
    

    /////////////////////////////////////////////////
    //(5)after data transfer done
    /////////////////////////////////////////////////
ncq_handler_over:
    clear_dma_run;
    SFR_CHP_en = DEF_CHP_EN;
    SFR_watch_dog_high = 0x00;
    return;
}



//This is the normal SET_DEV_BITS
void ncq_set_dev_bits()
{
    /////////////////////////////////////////////////
    //(1)transmit SET_DEV_BITS FIS
    /////////////////////////////////////////////////
    sata.quick_sactive = quick_sactive_clear;
    
    //firmware search for a TAG whose status is TAG_DONE.
    g_tmp[0] = 0x00;
    g_tmp_addr = NCQ_TABLE_BASE;
    for(i=0;i<NCQ_MAX_TAG;i++)
    {
        if(read_code(g_tmp_addr) == TAG_DONE)
        {
            //set the sactive
            sata.quick_sactive = i;
            g_tmp[0] = 0x01; //0x01 means find DONE tag
        }
        g_tmp_addr = g_tmp_addr + NCQ_LINE_LEN;
    }
    //if not pending tag, just return;
    if(g_tmp[0]==0x00)return;
    
    


    /////////////////////////////////////////////////
    //(2)If transmit FAIL, DO NOT retry and return
    /////////////////////////////////////////////////
    tx_fis_a1(status_good,error_no,int_set);
    if(g_fis_tx_abort)return;

    /////////////////////////////////////////////////
    //(3)If transmit Successfully, update NCQ TABLE: tag status 
    //                  => change it free now
    /////////////////////////////////////////////////
    enable_code_program;
    g_tmp_addr = NCQ_TABLE_BASE;
    for(i=0;i<NCQ_MAX_TAG;i++)
    {
        if(read_code(g_tmp_addr) == TAG_DONE)
        {
            write_code(g_tmp_addr,TAG_FREE);
        }
        g_tmp_addr = g_tmp_addr + NCQ_LINE_LEN;
    }
    disable_code_program;
    return;
}



//respond the read log ext
void ncq_read_log()
{
    if(sata.fis_rcv_content[4*1+0] == queue_error_log) 
    {
        tx_fis_34(status_good, error_no,int_set);
        read_queue_error_log();
    }
    else if(sata.fis_rcv_content[4*1+0] == ncq_management_log) 
    {
        tx_fis_34(status_good,error_no,int_set);
        read_ncq_management_log();
    }
    else tx_fis_34(status_bad,error_abort,int_set);
}

void ncq_management_hander()
{
  if(((sata.fis_rcv_content[4*1+0]>>3) == (sata.fis_rcv_content[4*3+0]>>3)) || ((sata.fis_rcv_content[4*0+3]>>3) >0x03))  //if tag == ttag or if is an undefine abort type
    {
      g_ncq_halt = 1;
      tx_fis_34(status_bad,error_abort,int_set);
      return;
    }
  else tx_fis_34(status_good,error_no,int_set);    
  abort_ncq();
}

void abort_ncq()
{
  g_tmp_addr = NCQ_TABLE_BASE;
    for(i = 0;i<NCQ_MAX_TAG;i++)
      {
        if(read_code(g_tmp_addr) != TAG_FREE) 
        switch(sata.fis_rcv_content[4*0+3]>>4)    
          {
            case ncq_abort_all:
              sata.quick_sactive = i; break;
            case ncq_abort_streaming:
              if(read_code(g_tmp_addr+ADDR_ICC) != 0) sata.quick_sactive = i; 
              break;
            case ncq_abort_nonstreaming:
              if(read_code(g_tmp_addr+ADDR_ICC ==0)) sata.quick_sactive = i;
              break;
            case ncq_abort_selected:
              if(sata.fis_rcv_content[4*1+0] == i) sata.quick_sactive; 
              break;
          }
        g_tmp_addr += NCQ_LINE_LEN;       
      }   
    tx_fis_a1(status_good,error_no,int_set);
    g_tmp_addr = NCQ_TABLE_BASE;

    for(i = 0;i<NCQ_MAX_TAG;i++)
      {
        enable_code_program;
          if(sata.quick_sactive & 0x01)
        write_code(g_tmp_addr,TAG_FREE);
          g_tmp_addr += NCQ_LINE_LEN;
          sata.quick_sactive = sata.quick_sactive>>1;
       disable_code_program;      
      }
}

void read_queue_error_log()
{ 
   BYTE checksum;
   u16 t;
   reset_engine();
       
   g_ncq_halt = 0;
   SFR_ext_sram_addr = 0x0000;                    
   SFR_ext_sram_cntl = 0x10; //enable auto increese of the sram address
   SFR_ext_sram_data = g_tmp_tag;        
   SFR_ext_sram_data = 0x00;              //reserve
   SFR_ext_sram_data = g_tmp[ADDR_STATUS];
   SFR_ext_sram_data = error_abort;
   SFR_ext_sram_data = g_tmp[ADDR_LBA0];
   SFR_ext_sram_data = g_tmp[ADDR_LBA1];
   SFR_ext_sram_data = g_tmp[ADDR_LBA2];
   SFR_ext_sram_data = g_tmp[ADDR_DEVICE];
   SFR_ext_sram_data = g_tmp[ADDR_LBA3];
   SFR_ext_sram_data = g_tmp[ADDR_LBA4];
   SFR_ext_sram_data = g_tmp[ADDR_LBA5];
   SFR_ext_sram_data = 0x00;           //reserve
   SFR_ext_sram_data = g_tmp[ADDR_SECCNT_LOW];
   SFR_ext_sram_data = g_tmp[ADDR_SECCNT_HIGH]; 
 
   for(t=0;t<511-14;t++)
     {
       SFR_ext_sram_data = 0x00;
     }
   SFR_ext_sram_addr = 0x0000;
   SFR_ext_sram_cntl = 0x00;
   checksum = 0x00;
   for(i=0;i<14;i++)
     {
       checksum += SFR_ext_sram_data;
       SFR_ext_sram_addr++;
     }
     
   checksum -= 0x00;   //we must guarantee that the sum of 512byte is 0x00;
   SFR_ext_sram_addr = 0x01ff;
   SFR_ext_sram_data = checksum;
   if(SFR_FIS0_2 == ata_read_log_ext)
     {
       tx_fis_5f(status_drq, error_no, pm_pio_read, 0x01, 0x00, status_good);
      
     }
     //if is a read log dma ext command
    sata_burst(PIO_READ,0x01);
    if(sata_burst_abort) g_ncq_halt = 1;
    reset_engine();
}

void read_ncq_management_log()
{
  BYTE checksum;
  u16 t;
  reset_engine();
  g_ncq_halt = 0;
  SFR_ext_sram_addr = 0x0000;
  SFR_ext_sram_cntl = 0x10;
  for(t=0;t<511;t++)
    {
      SFR_ext_sram_data = 0x00;
    }
  SFR_ext_sram_addr = 0x0003;
  SFR_ext_sram_data = 0x1f;       //support abort ncq subcommand;
  checksum = 0xe1;                      //we must guarantee that the sum of 512byte is 0x00;
  SFR_ext_sram_addr = 0x01ff;
  SFR_ext_sram_data = checksum;

  if(SFR_FIS0_2 == ata_read_log_ext)         
    {
      tx_fis_5f(status_drq, error_no, pm_pio_read, 0x01, 0x00, status_good);
    }
  else                         //if is a read log dma ext command
    sata_burst(PIO_READ,0x01);
    if(sata_burst_abort) g_ncq_halt = 1; 
    reset_engine();
}
#endif


