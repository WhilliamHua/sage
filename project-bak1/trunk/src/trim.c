#ifdef SUPPORT_TRIM

void trim_handler()
{
    u16_t idata jj;
    u16_t idata sram_addr,addr_temp;
    reset_engine();
    sram_addr.word=0;
    SFR_sata_burst_cnt = sata.FIS_seccnt;
    SFR_sata_burst_base = 0x0000;
    SFR_sata_burst_cntl = SFR_sata_burst_cntl_run | SFR_sata_burst_cntl_dma_act;
    while(!sata_burst_done); 
    if(sata_burst_abort){tx_fis_34(status_good,error_no,int_set);goto end_of_trim;}
    clear_burst_run;
    
    jj.word= 64*sata.FIS_seccnt;    
    SFR_ext_sram_addr = 0x0000;
    SFR_ext_sram_cntl = 0x10;  //enable addr auto add one
    sram_addr.word= 0;
    while(sram_addr.word< jj.word)    //////////////////////////////??????? affirm that 64*sata.fis_seccnt is right
    {
        addr_temp.word = sram_addr.word;
        SFR_ext_sram_addr = (sram_addr.word<<3)+0x06;
        if((SFR_ext_sram_data == 0x00)&&(SFR_ext_sram_data == 0x00)) {break; } //check the end of trim table
        sum.Number = 0;
        for(;sram_addr.word< jj.word;sram_addr.word++)
        {
            SFR_ext_sram_addr = (sram_addr.word<<3);  
            fetch_table_lba();
            trim_addr_offset.No[3] = SFR_ext_sram_data;
            trim_addr_offset.No[2] = SFR_ext_sram_data;
            trim_addr_offset.No[1] = 0x00;
            trim_addr_offset.No[0] = 0x00;
            lba_adder();
            sum.Number += trim_addr_offset.Number; 
            if(sram_addr.word == ( jj.word-1)){goto continue_end;}
            if(cmp_lba()) {break;}    //if lba discontinue
        } 
        continue_end:    
         //check if the lba exceed the os visible area
        if(trim_check_lba()){tx_fis_34(status_good,error_no,int_set);goto end_of_trim;}
        fetch_block_border(addr_temp); 
        sram_addr.word++;
        if(sum.Number <= trim_addr_offset.Number) {continue;}
        sum.Number -= trim_addr_offset.Number;  
		send_trim_cmd(sum); 
   }
    SFR_ext_sram_cntl = 0x00;  //disable addr auto add one
    tx_fis_34(status_good,error_no,int_set);
    end_of_trim:
    reset_engine();
}
void fetch_block_border(u16_t idata addr)
{
    ///////////////////////////////////////////////////////////////
    //input:        (1)addr: the begin addr of continuous area in the sram. 
    //output:      (1)sata.SUP_LBA: the begin boundary of the big block lba.
    //                (2)trim_addr_offset: begin of big block boundary - continuous area's begin addr
    ///////////////////////////////////////////////////////////////
    
    u32_t idata tmp,buf;
    tmp.Number = 0;
    buf.Number = 0;
    //fetch origin lba from the trim table
    SFR_ext_sram_addr = (addr.word<<3);   //the sram_addr's unit is 8byte
    fetch_table_lba();
    //fetch block size
    tmp.No[3]= BLOCK_SIZE_L;
    tmp.No[2]= BLOCK_SIZE_H; 
    
    //compute (tmp-1)
    if(CHP_NUMBER != 0x05)
    {
        tmp.Number = tmp.Number<<(CHP_NUMBER>>1);   //note that chp_number can only be 0x01, 0x02, 0x04
        tmp.Number = tmp.Number-1;
        enable_fetch_lba;
        trim_addr_offset.No[3] = sata.SUP_LBA0 & tmp.No[3];
        trim_addr_offset.No[2] = sata.SUP_LBA1 & tmp.No[2];
        trim_addr_offset.No[1] = sata.SUP_LBA2 & tmp.No[1];
        trim_addr_offset.No[0] = 0x00;
        disable_fetch_lba;
        if((trim_addr_offset.No[3]==0)&&(trim_addr_offset.No[2]==0)&&(trim_addr_offset.No[1]==0)) {return; }//lba just to be the border, so offset=0
        else
        {
            tmp.Number++;
            trim_addr_offset.Number = tmp.Number-trim_addr_offset.Number;
            lba_adder();
        }           
    }
    else     //if chp_number == 0x05;
    {  
        sata.CHP_stripe = 0xf1;
        buf.Number = tmp.Number;
        tmp.Number -= 1;
        enable_fetch_lba;
        trim_addr_offset.No[3] = sata.SUP_LBA0 & tmp.No[3];
        trim_addr_offset.No[2] = sata.SUP_LBA1 & tmp.No[2];
        trim_addr_offset.No[1] = sata.SUP_LBA2 & tmp.No[1];
        trim_addr_offset.No[0] = 0x00;
        disable_fetch_lba;
        tmp.Number = buf.Number;   //both tmp and buf equal to block_sizre
        while(buf.Number != 0x01)
        {
            lba_shift_right();
            buf.Number = buf.Number >> 1;
        }               
        buf.Number = 0;       
        if((trim_addr_offset.No[3] != 0x00)||(trim_addr_offset.No[2] !=0x00)||(trim_addr_offset.No[1] != 0x00)) {auto_increase_lba;buf.Number+=tmp.Number;}
        do{  
            if(sata.chp_bus_sel == 0x01) break;
            buf.Number += tmp.Number;
            auto_increase_lba;
        }while(1);
        sata.CHP_stripe = DEF_CHP_STRIPE;
        trim_addr_offset.Number = buf.Number-trim_addr_offset.Number;
        SFR_ext_sram_addr = (addr.word<<3);   //the sram_addr's unit is 8byte
        fetch_table_lba();
        lba_adder();     
    } 
}
void lba_adder()
{
    u8 idata buf[2];
    for(i=0;i<trim_addr_offset.No[3];i++)  auto_increase_lba;
    enable_fetch_lba;
    buf[0] = sata.SUP_LBA0;
    sata.SUP_LBA0 = sata.SUP_LBA1;
    sata.SUP_LBA1 = sata.SUP_LBA2;
    sata.SUP_LBA2 = sata.SUP_LBA3;
    sata.SUP_LBA3 = sata.SUP_LBA4;
    sata.SUP_LBA4 = sata.SUP_LBA5;
    for(i=0;i<trim_addr_offset.No[2];i++) auto_increase_lba;
    buf[1] = sata.SUP_LBA0;
    sata.SUP_LBA0 = sata.SUP_LBA1;
    sata.SUP_LBA1 = sata.SUP_LBA2;
    sata.SUP_LBA2 = sata.SUP_LBA3;
    sata.SUP_LBA3 = sata.SUP_LBA4;
    sata.SUP_LBA4 = sata.SUP_LBA5;
    for(i=0;i<trim_addr_offset.No[1];i++) auto_increase_lba;
    sata.SUP_LBA5 = sata.SUP_LBA3;
    sata.SUP_LBA4 = sata.SUP_LBA2;
    sata.SUP_LBA3 = sata.SUP_LBA1;
    sata.SUP_LBA2 = sata.SUP_LBA0;
    sata.SUP_LBA1 = buf[1];
    sata.SUP_LBA0 = buf[0];
    disable_fetch_lba;
    triger_bus_sel;
}

u8 cmp_lba()
{  
    enable_fetch_lba;
    if(sata.SUP_LBA0 != SFR_ext_sram_data) goto discontinue;
    if(sata.SUP_LBA1 != SFR_ext_sram_data) goto discontinue;
    if(sata.SUP_LBA2 != SFR_ext_sram_data) goto discontinue;
    if(sata.SUP_LBA3 != SFR_ext_sram_data) goto discontinue;
    if(sata.SUP_LBA4 != SFR_ext_sram_data) goto discontinue;
    if(sata.SUP_LBA5 != SFR_ext_sram_data) goto discontinue;
    disable_fetch_lba;
    return lba_continue;
    discontinue:
    disable_fetch_lba;
    return lba_discontinue;
}
void send_trim_cmd(u32_t idata sec_cnt)
{
    ////////////////////////////////////////////////
    //input: (1) SUP_LBA: boundary of the big block address
    //       (2) sec_cnt: continuous area length - (begin of big block boundary - begin address of the continuous area)
    ////////////////////////////////////////////////
    u8 j;
    u32_t idata block_size; 
    block_size.Number= 0;
    //save lba
    block_size.No[3] = BLOCK_SIZE_L;
    block_size.No[2] = BLOCK_SIZE_H;
    trim_addr_offset.Number = (1<<((sata.CHP_stripe&0x0f)-1));
    if(CHP_NUMBER != 0x05)
    {
        block_size.Number = block_size.Number<<(CHP_NUMBER>>1); //note that the chp_num here can only be 0x01, 0x02, 0x04
        while(block_size.Number != 0x01)
        {
            block_size.Number = block_size.Number>>1;
            sec_cnt.Number = sec_cnt.Number>>1;
        }
    }
    else
    {
        save_lba();
        while(block_size.Number != 0x01)
        {
            block_size.Number = block_size.Number >>1;
            sec_cnt.Number = sec_cnt.Number >> 1;         
        }
        sata.CHP_stripe = 0xf1;
        sata.SUP_LBA0 = sec_cnt.No[3];
        sata.SUP_LBA1 = sec_cnt.No[2];
        sata.SUP_LBA2 = sec_cnt.No[1];
        sata.SUP_LBA3 = sec_cnt.No[0];
        sata.SUP_LBA4 = 0x00;
        sata.SUP_LBA5 = 0x00;
        delay(30);
        //when we didn't  set enable_fetch_lba, the value we got from sata.SUP_LBA register is the real LBA/5.
        disable_fetch_lba;
        sec_cnt.No[3] = sata.SUP_LBA0;
        sec_cnt.No[2] = sata.SUP_LBA1;
        sec_cnt.No[1] = sata.SUP_LBA2;
        sec_cnt.No[0] = sata.SUP_LBA3;
		sata.CHP_stripe_h = DEF_CHP_STRIPE_H;
        sata.CHP_stripe = DEF_CHP_STRIPE;
        recover_lba();
    }
    block_size.Number= 0;
    block_size.No[3] = BLOCK_SIZE_L;
    block_size.No[2] = BLOCK_SIZE_H;  
    if(sec_cnt.Number == 0) return;
    while(block_size.Number != 1)
    {
        sec_cnt.Number = sec_cnt.Number<<1;
        block_size.Number = block_size.Number>>1;
    }

    SFR_CHP_en = DEF_CHP_EN;    
    triger_bus_sel;
    stop_all_chp;
    wait_chp_ready();
    SFR_CHP_en = 0x01;
    for(j=0;j<CHP_NUMBER;j++)
    {      
        trim_burst(sec_cnt);       
        lba_adder();                                      
        SFR_CHP_en = SFR_CHP_en<<1;
    }        
    SFR_CHP_en = DEF_CHP_EN;
    wait_chp_ready();
}
void trim_burst(u32_t idata block_num)
{
    SFR_ext_sram_addr = 0x4006;
    SFR_ext_sram_cntl = 0x10;	//enable auto increese of the sram address
    for(i=0;i<4;i++)
    {
        SFR_ext_sram_data = block_num.No[i];
    } 
    SFR_dma_max_num = 0x0001;     
    SFR_dma_base = 0x4000;
    sata.CHP_cmd = CHP_CMD_TRIM; 
    SFR_dma_cntl = SFR_dma_cntl_run;
    while(!dma_done);
    SFR_dma_cntl = 0x00;                  //clear running
}
void lba_shift_right()
{
    save_lba();
    g_lba[0] = (g_lba[0]>>1)+(g_lba[1]<<7);
    g_lba[1] = (g_lba[1]>>1)+(g_lba[2]<<7);
    g_lba[2] = (g_lba[2]>>1)+(g_lba[3]<<7);
    g_lba[3] = (g_lba[3]>>1)+(g_lba[4]<<7);
    g_lba[4] = (g_lba[4]>>1)+(g_lba[5]<<7);
    g_lba[5] = g_lba[5]>>1;
    recover_lba();
}
void fetch_table_lba()
{
    sata.SUP_LBA0 = SFR_ext_sram_data;
    sata.SUP_LBA1 = SFR_ext_sram_data;
    sata.SUP_LBA2 = SFR_ext_sram_data;
    sata.SUP_LBA3 = SFR_ext_sram_data;
    sata.SUP_LBA4 = SFR_ext_sram_data;
    sata.SUP_LBA5 = SFR_ext_sram_data;     
}

u8 trim_check_lba()
{
    u8 idata i;
    save_lba();
    for(i=0;i<6;i++)
    {
        if(g_lba[5-i]>sata.max_lba[5-i]) return 1;
        if(g_lba[5-i]<sata.max_lba[5-i]) return 0;
    }
    return 1;
}
#endif//#ifdef SUPPORT_TRIM
