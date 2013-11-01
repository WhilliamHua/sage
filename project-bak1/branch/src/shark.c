#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "type_def.h"
#include "REG52x2.h"
#include "common.h"
#include "reg_def.h"
#include "ncq.h"
#include "buf_cmd.h"
#include "sata_proc.h"
#include "ata_cmd.h"
#include "shark.h"
#include "uart.h"
#include "chp.h"
#include "smart.h"
#include "trim.h"
#include "security.h"

#include "uart.c"
#include "sata_proc.c"
#include "ata_cmd.c"
#include "ncq.c"
#include "buf_cmd.c"
#include "debug.c"
#include "chp.c"
#include "smart.c"
#include "trim.c"
#include "security.c"

//u8 security_nd_chk ;

void main()
{	
	 init(0x00);
	 
	 /* wait EFM32 to choose a partition or ATA password is supported*/
	 #ifdef GPIO_security
	GPIO_security_init();	 
	 //security_nd_chk = security_cmd_info_read(scl_security_nd_chk);//security function need or not 
	 //myprintf("\nsysmode:%c",security_nd_chk);
	 
	 //while(/*(security_nd_chk=='Y') && */(!g_security_enable) && (security_mode !=show_master) && (security_mode !=show_user) && (security_mode !=show_all))
	 //	{
	 //	GPIO_security_cmd(1);
	//	myprintf("\nwait chk");
	//	uart_show=0x1;
	// }
	 //init();
	 #endif
	 
	   
    while(1)
    {
  
	#ifdef SUPER_ERASE

	#ifdef GPIO_security
	if((security_mode !=show_master) && (security_mode !=show_user) && (security_mode !=show_all))
	{
		g_security_lock=1;
	 	GPIO_security_cmd(0x01);
	}
	else
	{
		g_security_lock=0;
		if(g_SYS_YD==0x01)
		{
		  GPIO_security_cmd(0x00);
		}
		else
		{  	myprintf("\nwhile erase");
		  GPIO_security_cmd(0x01);
		}
	}
		
	#endif
	
	
	if((security_erase==show_master) ||(security_erase==show_user))
	 	{
		security_cmd_info_write('N', scl_security_nd_chk);//need reconfig
		myprintf("\nes0");
		intial_data_partition(0);//erase partition
		security_erase=0x00;
		myprintf("\ned0");
	}
	else if(security_erase==show_all)
		{
		security_cmd_info_write('N', scl_security_nd_chk);//need reconfig
		myprintf("\nes0");
		intial_data_partition(1);//erase partition
	  	security_erase=0x00;
		myprintf("\ned1");
	}


	 if(uart_show)
	 	{
		  if(security_mode == show_master)
		  	{
			myprintf("\nmaster mode");
			uart_show=0;
		  }
		  else if(security_mode == show_user)
		  	{
			myprintf("\nuser mode");
			uart_show=0;
		  }
		  else if(security_mode == show_all)
		  	{
			myprintf("\nzsd mode");
			uart_show=0;
		  }
		  tx_fis_A1(status_good, error_no, int_set);

		  init(0x01);
	 	}

	 #endif
	 
	#ifdef security_debug
	  myprintf("\rSV:%x",SFR_spi_io);
	#endif

	#ifdef BUF_CMD_EN
            //todo: if NCQ error, need additional control here???
                cmd_buffer_check();
        #endif
        //check new fis received from host(exclude data fis)
        //sata.led0_cntl = 0x80;      //rom is using 0xd4
        if(g_current_fis_num != SFR_fis_num)
        { 
            //sata.led0_cntl = 0x00;      //rom is using 0xd4
            sata.led0_blink = 0x44;
            //myprintf("\nSR0:%x",SFR_FIS0_1);
            //todo: add an led1 here.
            //myprintf("\nfine a new cmd:%x",SFR_FIS0_2);
            
            g_current_fis_num = SFR_fis_num;
            
            //clear the fis27 flag.
            sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
            
            //if in SLEEP mode, only respond to SRST. Please refer to ATA8 Charpter 7.55.2
            
			//TODO: need to open this after this is tested.
            //if(g_power_mode == power_mode_sleep)goto check_control; 

			
            #ifdef SUPPORT_NCQ
            if(g_ncq_halt)
            {
               if(check_ncq_read_log);
               //else goto Over;
               else continue;
            }
			#endif
            
            //open the CHP clock
            chp_clock_on();
			
            #ifdef SUPPORT_SMART1
            if(g_smart_power_down_flag == 1)
            {
                updata_smart(smart_power_down_addr,0x01);
                g_smart_power_down_flag = 0;
                reset_engine();
            }
            #endif

            //this will load the sector count
            check_sec_cnt();
            //myprintf("\nsec_cnt:%x",g_sec_cnt.byte.l);
            check_lba();
		//myprintf("\nLBAchk0:%x");
		
            if(SFR_quick_cmd!=0)
            {
                #ifdef SUPPORT_SECURITY
                //analyze_states();
                g_security_prepared = 0;
                if(g_security_lock == 1) 
                {
                tx_fis_34(status_bad, error_abort,int_set);
                }
                else
                #endif
                {
                    #ifdef BUF_CMD_EN
                    if(SFR_quick_cmd & quick_cmd_write_dma)
                    {

                        //break buffer command flow:
                        //(1)continue command
                        //(2)chs mode command
                        //(3)counter > threshold
    					if(check_chs_cmd)
    					{
    						if(buf_not_empty) g_current_fis_num--;
    						else
    						{ 
    							chs_write_handle();
    							buf_check_cnt = 0x00;
    						}
    					}
                        else if(normal_cmd_cont || (g_sec_cnt.word == 0x0000) || (g_sec_cnt.byte.h!=0x00) || (g_sec_cnt.byte.l >BUF_SEC_CNT_THRESHOLD) ||(g_enable_write_cache == 0))
                        {
                             
                             if(buf_not_empty) g_current_fis_num--;
                             else
                             {
                                write_handler();
                                buf_check_cnt = 0x00;
                             }
                             
                        } 
                        else if(buf_check_cnt<BUF_START_CNT)
                        {
                            if(buf_not_empty) g_current_fis_num--;
                            else
                            {
                               write_handler();
                               buf_check_cnt++;
                            }
                        }
                        else
                        {      
                             //check_lba();
                             //the if will not happen
                             if(g_flag_lba_err)
                             {
                                 tx_fis_34(status_bad,error_abort,int_set);
                                 reset_engine();
                             }
                             else
                             {
                                 buf_sector_num.word = 0;
                                 buf_cmd_flag = DEF_CHP_EN;
                                 cmd_buffer_set_param();
                                 //TODO: add reset_engine here???
                                 g_cont_en = 0;
                             }
                         }
                    }
                    //need to flush all data in FIFO before other commands
                    else if(buf_not_empty)g_current_fis_num--;                     
                    #else
                    if(SFR_quick_cmd & quick_cmd_write_dma)
                    {
    					if(check_chs_cmd)chs_write_handle();
    					else write_handler();
                    }
                    #endif
                    
                                                      
                 
                    //(2)this is read_dma or read_dma_ext
                    else if(SFR_quick_cmd & quick_cmd_read_dma)
                    {  
                       // myprintf("\nr");
    				   if(check_chs_cmd)chs_read_handle();
                       else read_handler();
     
                    }
                    else if(SFR_quick_cmd & (quick_cmd_write_mul | quick_cmd_write_sec))  
                    {              
                        g_flag_dir = DIR_WRITE;
                        pio_write_handler();              
                     }   
                    
                    //(4)read pio: read_sector([ext]) or read_multiple([ext])
                    //else if((SFR_quick_cmd&quick_cmd_read_mul) || (SFR_quick_cmd&quick_cmd_read_sec));
                    else if(SFR_quick_cmd & (quick_cmd_read_mul | quick_cmd_read_sec))
                    {
                       g_flag_dir = DIR_READ;
                       pio_read_handler();
                    }
                    g_power_mode = power_mode_act;
                }
            }
            else if(SFR_FIS0_0 == 0x27)//this is H2D fis
            { 
                #ifdef BUF_CMD_EN
                if(buf_not_empty)g_current_fis_num--; 
                else if((SFR_FIS0_1 & 0x80) !=0) //this is a command, not control
                #else
                if((SFR_FIS0_1 & 0x80) !=0) //this is a command, not control
                #endif
                {
                    #ifdef SUPPORT_SECURITY
                    if(SFR_FIS0_2 != ata_security_erase_unit) g_security_prepared = 0;
                    #endif
                    if(SFR_FIS0_2 == ata_write_dma_fua_ext)
                    {
                        write_handler();
					    g_power_mode = power_mode_act; 
                    }
                    else if(SFR_FIS0_2 == ata_write_multi_fua_ext)
                    {
                        g_flag_dir = DIR_WRITE;
                        pio_write_handler(); 
						g_power_mode = power_mode_act;                        
                    }
                    
                    #ifdef SUPPORT_NCQ
                    else if(SFR_FIS0_2 == ata_read_fpdma)
                    {
                        g_flag_dir = DIR_READ;
                        ncq_resp_cmd();
                        g_power_mode = power_mode_act;
                    }
                    else if(SFR_FIS0_2 == ata_write_fpdma)
                    {
                        g_flag_dir = DIR_WRITE;
                        ncq_resp_cmd();
                        g_power_mode = power_mode_act;
                    }
                    #endif
                    else
                    {
                        reset_engine();
                        switch(SFR_FIS0_2)//this is the ata command index
                        {  
                        
                            //NCQ
                            #ifdef SUPPORT_NCQ
                        
                            case ata_ncq_management:
                            {
                                ncq_management_hander();
                                g_power_mode = power_mode_act;
                                break;
                            }
                        
                            case ata_read_log_ext:
                            {
                                ncq_read_log();
                                g_power_mode = power_mode_act;
                                break;
                            }        
                            case ata_read_log_dma_ext:
                            {
                                ncq_read_log();
                                g_power_mode = power_mode_act;
                                break;
                            }
                        
                            #endif
                        ///////////////////////////////////////////////////
                        ///////////BASIC
                        ///////////////////////////////////////////////////
                            case ata_nop:                   
                            {
                                //note: sub-code is used for overlapped feature support.
                                tx_fis_34(status_bad, error_abort,int_set); 
                                break;
                            }
                            case  ata_seek:	
                            {
                                //this is sent by host to signify the device that it may access the specified address in the following commands
                            
                                if(g_flag_lba_err) tx_fis_34(status_bad,error_abort, int_set);
                                else tx_fis_34(status_good,error_no,int_set);
                                g_power_mode = power_mode_act;
                                break;							
                            }
                            case ata_recalibrate:
                            {
                                //this is vendor specific - ATA4
                                tx_fis_34(status_good,error_no,int_set);
                                break;
                            }
                            case ata_exec_diag:
                            {
                                init_tx_fis();
                                tx_fis_34(status_bad&0xf2,0x01,int_set);       //0x01h indicated device0 passed and device1 not present
                                break;
                            }
                            case  ata_init_dev_para:	
                            {
                                //this is to set CHS translation.
                                //todo: change the setting of hardware operation: CHS translation.
                                init_dev_para();
                                break;							
                            }
                            case ata_flush_cache:
                            {
                            //TODO: flush the CHPs.
                            //This command is used by the host to request the device to flush the write cache. If there is data in the write
                            //cache, that data shall be written to the media. This command shall not indication completion until the data is
                            //flushed to the media or an error occurs.
                            //NOTE . This command may take longer than 30 s to complete.
                                flush_delay();
                                tx_fis_34(status_good,error_no,int_set);
                                g_power_mode = power_mode_act;
                                break;
                            }
                            case ata_flush_cache_ext:
                            {
                            	flush_delay();
                                tx_fis_34(status_good,error_no,int_set);
                                g_power_mode = power_mode_act;
                                break;
                            }
                            case ata_read_verify: //this command wasn't completed, the write protect error should be took into accout
                            {
                                if(g_flag_lba_err) tx_fis_34(status_bad,error_abort, int_set);
                                else tx_fis_34(status_good,error_no,int_set);
                                g_power_mode = power_mode_act;
                                break;	
                            }
                            case ata_read_verify_ext:  //this command wasn't completed, the write protect error should be took into accout
                            {
                                if(g_flag_lba_err) tx_fis_34(status_bad,error_abort, int_set);
                                else tx_fis_34(status_good,error_no,int_set);
                                g_power_mode = power_mode_act;
                                break;	
                            }

                            #ifdef SUPPORT_SMART

                            case ata_smart:
                            {
                                //myprintf("\nsmart_handler");
                                smart_handler();
                                break;
                            }
                        
                            #endif

                       
                        case ata_data_set_management:
                        {
                            #ifdef SUPPORT_TRIM
                           //#if 0
                                //if(check_sd_mode) tx_fis_34(status_good,error_no,int_set);
                                //else 
                                if((sata.FIS_seccnt_ext !=0)||(sata.FIS_seccnt >0x20)||((sata.FIS_seccnt_ext==0)&&(sata.FIS_seccnt==0)))
                                {
                                     tx_fis_34(status_good,error_no,int_set);
                                }
                                else  trim_handler();
                            #else
                                tx_fis_34(status_good,error_no,int_set);
                            #endif
                            
                            break;
                            
                        }
                        

                            ////////////////////////////////////////////////////
                            /////POWER MANAGEMENT FEATURE SET
                            ////////////////////////////////////////////////////
                            case ata_chk_power_mode:
                            {
                                sata.FIS_seccnt=g_power_mode;
                                tx_fis_34(status_good,error_no,int_set);
                            
                                break;
                            }
                            case ata_idle:
                            {
                                flush_delay();
                                g_power_mode=power_mode_idle;
                                tx_fis_34(status_good,error_no,int_set);
                                break;
                            } 
                            case ata_idle_imd:
                            { 
                                flush_delay();
                                g_power_mode=power_mode_idle;
                                tx_fis_34(status_good,error_no,int_set);
                                break;                                                                                                
                            }
                            case ata_sleep:
                            {  
                                flush_delay();
                                g_power_mode=power_mode_sleep;
                                tx_fis_34(status_good,error_no,int_set);
                                break;                                                                             
                            }
                            case ata_standby:
                            {
                                flush_delay();
                                g_power_mode=power_mode_standby;
                                tx_fis_34(status_good,error_no,int_set);                           
                                break; 
                            }
                            case ata_standby_imd:
                            {  
                                flush_delay();
                                g_power_mode=power_mode_standby;
                                tx_fis_34(status_good,error_no,int_set);
                                break;                                                                                                       
                            }
                        
                        ///////////////////////////////////////////////////////
                        ///////////////////////////////////////////////////////
                            case ata_identify_device:
                            {
                                identify_device();
                                break;
                            }
                            case ata_set_multiple:
                            { 
                                if(sata.fis_rcv_content[4*3+0] == 0x01)
                                {
                                // we support transmitting 1 sector per multiple operation only, so the identify_data[59] low will not change                      
                                /*
                                                        identify_data[59] &= 0xff00;
                                                        identify_data[59] |=0x0001; 
                                                        */
                                    tx_fis_34(status_good,error_no,int_set);
                                    break;
                                }  
                                else
                                {
                                    tx_fis_34(status_bad,error_abort,int_set);
                                    break;
                                }
                            }
                            case ata_set_feature:
                            {
							    set_feature();
                                //tx_fis_34(status_good,error_no,int_set);
                                break;
                            }
    					    case ata_security_freeze_lock:  //ok
                            {
                                #ifdef SUPPORT_SECURITY
                               if(g_security_lock == 1)
                                {
                                    tx_fis_34(status_bad, error_abort,int_set);
                                }
                                else
                                {
                                    if(g_security_enable == 0) security_state_flag = SEC2;
                                    else security_state_flag = SEC6;
                                    analyze_states();
                                    tx_fis_34(status_good,error_no,int_set);
                                }
                                #else
                                tx_fis_34(status_good,error_no,int_set);
                                #endif
                                break;	
                            }

                            //////////////////////////////////////////////////
                            #ifdef SUPPORT_SECURITY

			        case ata_security_erase_pre:   /* BIOS没有权限擦除硬盘，所以直接返回 */
                            {
                                if(g_security_frozen == 1)
                                {
                                    g_security_prepared = 0;
                                    tx_fis_34(status_bad, error_abort,int_set);
                                }
                                else
                                {
                                    //g_security_prepared = 1;
                                   // security_state_flag = SEC4;
                                    tx_fis_34(status_good,error_no,int_set);
                                }
                                //analyze_states();
                                break;
                            }

				case ata_security_erase_unit:       /* BIOS没有权限擦除硬盘，所以直接返回 */
                            {
                                if(g_security_frozen == 1)
                                {
                                    g_security_prepared = 0;
                                    tx_fis_34(status_bad, error_abort,int_set);
                                }
                                else
                                {
                                   /*if(g_security_prepared == 0) g_security_abort = 1;
                                    else security_erase_unit();
                                    if(g_security_abort == 1) 
                                    {
                                        g_security_abort = 0;
                                        tx_fis_34(status_bad, error_abort,int_set);
                                    }
                                    else 
                                    {
                                        g_security_prepared = 0;
                                        security_state_flag = SEC1;*/
                                        tx_fis_34(status_good,error_no,int_set);
                                    /*}    
                                    analyze_states();*/
                                }
                                break;
                            }
				
                            case ata_security_dis_pwd:      //ok
                            {
				   //此处删掉frozen下不能取消密码功能，dell 1014 进入bios的时候已经frozen了
				    if(/*(g_security_frozen == 1) ||*/ (g_security_lock == 1))
                                {
                                    g_security_prepared = 0;
                                    tx_fis_34(status_bad, error_abort,int_set);
                                }
                                else
                                {
                                    security_dis_pwd();
                                    if(g_security_abort == 1) 
                                    {
                                        g_security_abort = 0;
                                        tx_fis_34(status_bad, error_abort,int_set);
                                    }
                                    else 
                                    {
                                        security_state_flag = SEC1;
                                        tx_fis_34(status_good,error_no,int_set);
                                    }
                                    analyze_states();
                                }
                                break;
                            }
                            
                            case ata_security_set_pwd:  //ok
                            {
                                if((g_security_frozen == 1) )
                                {
                                    g_security_prepared = 0;
                                    tx_fis_34(status_bad, error_abort,int_set);
                                }
                                else
                                {
                                    security_set_pwd();
                                    if(g_security_abort == 1) 
                                    {
                                        g_security_abort = 0;
                                        tx_fis_34(status_bad, error_abort,int_set);
                                    }
                                    else tx_fis_34(status_good,error_no,int_set);
                                    analyze_states();
                                }
                                break;
                            }
                            
                            case ata_security_unlock:   //ok
                            {
                                //analyze_states();
				    	//myprintf("\nunlock");
                                security_unlock();
                                if(g_security_abort == 1) 
                                {
                                    g_security_abort = 0;
                                    tx_fis_34(status_bad, error_abort,int_set);
                                }
                                else 
                                {
                                    
                                    security_state_flag = SEC5;
                                    tx_fis_34(status_good,error_no,int_set);
                                }
                                analyze_states();
                                break;
                            }
                            #endif
                            ///////////////////////////////////////////////
                            
							#ifdef SUPER_ERASE
							case ata_down_microcode:
							{
                                if(sata.FIS_feature == 0xCC)//super erase
								{
								    intial_data_partition(1);//erase all
								   // security_erase_pa(1);
								    tx_fis_34(status_good,error_no,int_set);
                                    break;
                                }
								else 
								{
									tx_fis_34(status_bad, error_abort,int_set); 
									break;
								}
							}
							#endif

							#ifdef READ_ONLY
                            case ata_read_only:
                            {
                                choose_mode();
                                break;
                            }
                            #endif
							
                            default :
							{
                                tx_fis_34(status_bad, error_abort,int_set); 
                                break;
                            }
                        }
                    }
                }
                
                else //control command process 
                {
                    #ifdef BUF_CMD_EN
                    buf_data_pending = 0;
                    #endif
                    reset_engine();
                    //check_control:
                    //IF SRST
                    if((SFR_cmd_aux & 0x01)!=0) 
                    {
                        //wait until SRST is deasserted.
                        while((sata.fis_rcv_content[15] & 0x04)==0x04);
                        //clear the fis27 flag.
                        //sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
                        //hardware latch the SRST state, firmware clear it.
                        SFR_cmd_aux = 0x00;
                        g_current_fis_num = SFR_fis_num;
                        //only resonse when SRST is not set                        
                    }

                    //also response the other controls without SRST bit set.
                    if(g_power_mode != power_mode_sleep)
                    {
                        init_tx_fis();
						//reset link(bit2) and transport(bit3).
						//if SRST, we may recover from abnormal state
                        sata.rst_cntl = sata.rst_cntl | 0x0C;
                        delay(5);
						sata.ncq_cntl = (sata.ncq_cntl & (~ncq_cntl_new_fis27));
                        tx_fis_34(status_good,0x01,int_no);
                        #ifdef SUPPORT_SMART1
                        updata_smart(smart_softrst_num_addr,0x01);
                        reset_engine();
                        #endif
                    }
                }
            }
            else if(SFR_FIS0_0 == 0x58)
            {
                bist_handler();
            }
            SFR_watch_dog_high = 0x00;
        }
        else
        {
            //(1)if current loop has command, not handle DMA setup
            //(2)if current loop handle DMA setup, not send SET DEV BITS FIS
            //(3)otherwise send SET_DEV_BITS FIS
            #ifdef SUPPORT_NCQ
            ncq_handler();
            #endif
        }
		#ifdef SUPPORT_SMART1
        if(check_sd_mode) smart_card_crc();
		else smart_block_handle();
		#endif
        
        /*
		if(g_power_mode == power_mode_idle)
		{
			if(SFR_watch_dog_high >= 0x20)	g_power_mode = power_mode_standby;			
		}
		*/
		if((SFR_watch_dog_high > 0x30) && g_chp_active)chp_clock_off();
    }
    
}
