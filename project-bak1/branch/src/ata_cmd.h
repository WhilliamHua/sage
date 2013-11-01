#ifndef ATA_CMD_H
#define ATA_CMD_H




//BASIC feature
#define ata_nop						0x00
#define ata_seek					0x70
#define ata_recalibrate			 	0x10
#define ata_exec_diag				0x90
#define ata_init_dev_para			0x91
#define ata_flush_cache			    0xE7
#define ata_flush_cache_ext		 	0xEA
#define ata_read_verify				0x40
#define ata_read_verify_ext			0x42
#define ata_identify_device			0xEC	
#define ata_set_feature				0xEF
#define ata_set_multiple			0xC6
#define ata_down_microcode			0x92

//set feature subcmd
#define disable_sata_feature        0x90
#define enable_write_cache          0x02
#define disable_write_cache         0x82
#define enable_adv_power_manage     0x05
#define enable_power_up_standby     0x06
#define disable_adv_power_manage    0x85
#define disable_power_up_standby    0x86
#define enable_rw_verify            0x0b
#define disable_rw_verify           0x8b
#define enable_look_ahead           0x55
#define disable_look_ahead          0xaa
#define enable_auto_acoustic_manage   0x42
#define disable_auto_acoustic_manage  0xc2
#define enable_revert_pow_default    0x66
#define disable_revert_pow_default   0xcc
#define mode_transfer               0x03
    #define mode_ultra_dma          0x40       
    #define mode_mul_dma            0x20
	#define mode_pio_tran           0x08

// READ feature
#define ata_read_dma				0xC8
#define ata_read_dma_ext			0x25
#define ata_read_multi				0xC4
#define ata_read_multi_ext			0x29
#define ata_read_sector             0x20
#define ata_read_sector_ext         0x24
#define ata_read_fpdma              0x60
#define ata_write_fpdma             0x61
#define ata_ncq_management          0x63
#define subc_abort_ncq              0x00
#define ata_write_dma_fua_ext       0x3D
#define ata_write_multi_fua_ext     0xCE


//for pio
#define pio_write_begin    0x00
#define pio_write_next  0x40


//WRITE feature
#define ata_write_dma               0xCA
#define ata_write_dma_ext           0x35
#define ata_write_multi             0xC5
#define ata_write_multi_ext         0x39
#define ata_write_sector            0x30
#define ata_write_sector_ext        0x34

//Power management feature set 
#define ata_chk_power_mode          0xE5
#define ata_idle                    0xE3
#define ata_idle_imd                0xE1
#define ata_sleep                   0xE6
#define ata_standby                 0xE2
#define ata_standby_imd             0xE0

//security Mode feature set option
#define ata_security_dis_pwd        0xF6
#define ata_security_erase_pre      0xF3
#define ata_security_erase_unit     0xF4
#define ata_security_freeze_lock    0xF5
#define ata_security_set_pwd        0xF1
#define ata_security_unlock         0xF2


#define ata_smart                   0xB0

//TRIM
#define ata_data_set_management  0x06

//User define
#define ata_read_only                   0x96
#define read_only_enable            0x55
#define read_only_disable           0x00



//Streaming feature set
#define ata_configure_stream        0x51
#define ata_read_stream_ext         0x2B
#define ata_read_stream_dma_ext	    0x2A
#define ata_read_log_ext            0x2F
#define ata_read_log_dma_ext        0x47
#define ata_write_stream_ext        0x3B
#define ata_write_stream_dma_ext    0x3A
#define ata_write_log_ext           0x3F
#define ata_write_log_dma_ext       0x3F

#define ata_dma_read                0xc8
#define ata_dma_write               0xca

#define status_good	                0x50
#define status_bad                  0x51
#define error_no                    0x00
#define error_abort	                0x04
#define int_set	                    0xff
#define int_no			            0x00


//for pio setup fis
#define status_drq                  0x58    //only DRQ set
#define status_bsy                  0xd0    //only BSY set 

#define pm_pio_read	                0xe0
#define pm_pio_write                0x00        



#define max_sec_per_head		    0x3f
#define max_head_per_cylnd		    0x10
#define max_cylnd	                0x3fff



#define max_cap_chs	                0xFBFC10        //CHS must be supported if capacity no greater than 16,514,064 sector


#define set_burst_run_rd SFR_sata_burst_cntl = 0xd0
#define set_burst_run_wr SFR_sata_burst_cntl = 0x90
#define clear_burst_run SFR_sata_burst_cntl = 0x10
// dma_active_auto is set
// without preread, since it does not help much for speed.
// and we want to do NCQ continue.
#ifdef ONLY_SUP
	#define set_dma_run_rd set_burst_run_rd
	#define set_dma_run_wr set_burst_run_wr
	#define clear_dma_run  clear_burst_run
	#define clear_chp_run SFR_dma_cntl = 0x00
#else
	#define set_dma_run_rd SFR_dma_cntl = 0xd8
	#define set_dma_run_wr SFR_dma_cntl = 0x98
	#define clear_dma_run SFR_dma_cntl = 0x18
#endif
#define stop_all_chp sata.CHP_cmd= CHP_CMD_STOP
#define auto_increase_lba SFR_lba_comp = lba_increase_one
#define auto_decrease_lba SFR_lba_comp = lba_decrease_one

#define check_chs_cmd ((SFR_cmd_aux & cmd_aux_lba_mode)==0x00)

#define SRST_pending  ((SFR_cmd_aux & 0x01)!=0)

void reset_engine_read_write();
void reset_engine();

void check_lba();
void check_sec_cnt();

void read_handler();
void write_handler();

void pio_read_handler();
void pio_write_handler();


void identify_device();
void set_feature();

void chs_read_handle();
void chs_write_handle();
void soft_lba_add();
u8 chs_cmp_lba();

#ifdef READ_ONLY
void choose_mode();
#endif
//BOOL check_power_management();

BYTE gen_check_sum();

u8 g_lba_add[4];

void init_dev_para(); 

#ifdef SUPER_ERASE
void intial_data_partition(u8 all_partial);
#endif

#endif
