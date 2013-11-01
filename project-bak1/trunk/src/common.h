#ifndef COMMON_H
#define COMMON_H


/////////////////////////////////////////////////////////////////////
//(1)debug purpose
//the following should be commented for product firmware
/////////////////////////////////////////////////////////////////////
//#define DEBUG
//#define ONLY_SUP
//#define DELAY_DIS
#define SATA_1P5G_ONLY
//#define SIM

#define ENABLE_PRINT_CHP
//#define ENABLE_PRINT
#define ENABLE_UART_INT
//#define ENABLE_CHP_INT

#ifdef ENABLE_PRINT_CHP
#define ENABLE_CHP_INT
#define ENABLE_PRINT
#endif

#ifdef ENABLE_PRINT
#define myprintf printu
#else
#define myprintf ;/\
/printu
#endif

//#define security_debug
//#define security_debug_chs

//#define SUPPORT_NCQ
//#define BUF_CMD_EN
#define ENABLE_SUP_CLK_OFF

//#define READ_ONLY
#define SUPER_ERASE

#define SUPPORT_SMART
#define SUPPORT_SMART1
#define SUPPORT_TRIM
#define VOL_DET_EN

#define SUPPORT_SECURITY
#define SUPPORT_SECURITY1


//even recieve contine command, using buffering flow control
//not enalbe this, this will hurt the performance
//#define BUF_CMD_ADD_CONT


#define MAX_CHP_NUMBER 5
#define CHP_MIN_SEL 0x01

/////////////////////////////////////////////////////////////////////
//(2)setting
/////////////////////////////////////////////////////////////////////
//#define CHP_SET_SELF

#ifdef CHP_SET_SELF
    #define DFT_CHP_CLOCK_SET 0x83
    
    #define CHP_NUMBER 1
    #ifndef CHP_NUMBER
    #error "CHP_NUMBER not defined"
    #elif CHP_NUMBER == 1
    #define DEF_CHP_EN 0x01
    #define CHP_MAX_SEL 0x01
    #define DEF_CHP_STRIPE 0x00
    #define DEF_CHP_STRIPE_H 0x00
    #elif CHP_NUMBER == 2
    #define DEF_CHP_EN 0x03
    #define CHP_MAX_SEL 0x02
    #define DEF_CHP_STRIPE 0x34
    #define DEF_CHP_STRIPE_H 0x00
    #elif CHP_NUMBER == 4
    #define DEF_CHP_EN 0x0f
    #define CHP_MAX_SEL 0x08
    #define DEF_CHP_STRIPE 0xf5
    #define DEF_CHP_STRIPE_H 0x00
    #elif CHP_NUMBER == 5
    #define DEF_CHP_EN 0x1f
    #define CHP_MAX_SEL 0x10
    #define DEF_CHP_STRIPE 0xf1
    #define DEF_CHP_STRIPE_H 0xff
    #elif CHP_NUMBER == 6
    #define DEF_CHP_EN 0x10
    #define DEF_CHP_STRIPE 0x00
    #define DEF_CHP_STRIPE_H 0x10
    #else
    #error: "CHP_NUMBER is defined, but invalid."
    #endif
     #define SMART_BASE_ADDR0 (read_code(0x7fbe))
    #define SMART_BASE_ADDR1 (read_code(0x7fbf))
    #define SMART_BASE_ADDR2 (read_code(0x7fc0))
    #define SMART_BASE_ADDR3 (read_code(0x7fc1))
    #define SMART_BASE_ADDR4 (read_code(0x7fc2))
    #define SMART_BASE_ADDR5 (read_code(0x7fc3))
    #define MAX_LBA0 (read_code(0x7fc4))
    #define MAX_LBA1 (read_code(0x7fc5))
    #define MAX_LBA2 (read_code(0x7fc6))
    #define MAX_LBA3 (read_code(0x7fc7))
    #define MAX_LBA4 (read_code(0x7fc8))
    #define MAX_LBA5 (read_code(0x7fc9))
	#define BLOCK_SIZE_L (read_code(0x7fca))
    #define BLOCK_SIZE_H (read_code(0x7fcb))
#else

	#define LOW_VOL_DET_SET (read_code(0x7fb2))
	#define SMART_PE_0	(read_code(0x7fb3))
	#define SMART_PE_1	(read_code(0x7fb4))
	#define SMART_PE_2	(read_code(0x7fb5))
	#define SMART_PE_3	(read_code(0x7fb6))
	#define DEF_REAL_CHP_STRIPE (read_code(0x7fb7))
    #define CHP_NUMBER (read_code(0x7fb8))
    #define CHP_MAX_SEL (read_code(0x7fb9))
    #define DEF_CHP_EN (read_code(0x7fba))
    #define DEF_CHP_STRIPE (read_code(0x7fbb))
    #define DEF_CHP_STRIPE_H (read_code(0x7fbc))
    #define DFT_CHP_CLOCK_SET (read_code(0x7fbd))
    
    #define SMART_BASE_ADDR0 (read_code(0x7fbe))
    #define SMART_BASE_ADDR1 (read_code(0x7fbf))
    #define SMART_BASE_ADDR2 (read_code(0x7fc0))
    #define SMART_BASE_ADDR3 (read_code(0x7fc1))
    #define SMART_BASE_ADDR4 (read_code(0x7fc2))
    #define SMART_BASE_ADDR5 (read_code(0x7fc3))
    #define MAX_LBA0 (read_code(0x7fc4))
    #define MAX_LBA1 (read_code(0x7fc5))
    #define MAX_LBA2 (read_code(0x7fc6))
    #define MAX_LBA3 (read_code(0x7fc7))
    #define MAX_LBA4 (read_code(0x7fc8))
    #define MAX_LBA5 (read_code(0x7fc9))
    #define BLOCK_SIZE_L (read_code(0x7fca))
    #define BLOCK_SIZE_H (read_code(0x7fcb))

#endif

//clock setting, eventually this will be from utility
//#define DFT_CHP_CLOCK_SET 0xCA
//#define DFT_CHP_CLOCK_SET 0xCC
//#define DFT_CHP_CLOCK_SET 0xC3
//#define DFT_CHP_CLOCK_SET 0xd3
//#define DFT_CHP_CLOCK_SET 0xc9



//////////////////////////////////////////////////////////////////////
//(3)macro functions
//////////////////////////////////////////////////////////////////////

//delay time: 37*13.33ns = 500ns = 0.5us per count in 3G mode.
//                                               = 1us per count in 1.5G mode
void delay(cnt) 
{
    u16 i;
	for(i=0;i< cnt;i++) ;
}

//in 3G mode.
void delay_ms(cnt) 
{
	u16 i;
	for(i=0;i< cnt;i++)
	{
	    delay(2012);
	}
}

//all chps are ready
#define chp_is_ready ((SFR_CHP_rdy & SFR_CHP_en) == SFR_CHP_en)

//all chps not ready
#define chp_not_ready ((SFR_CHP_rdy & SFR_CHP_en) == 0x00)

#define normal_cmd_cont ((SFR_cmd_aux & 0xc0) == 0xc0)
#define normal_cmd_not_cont ((SFR_cmd_aux & 0xc0) != 0xc0)
							
#define enable_code_program TS8052_PCON  |= PCON_PWM
#define disable_code_program TS8052_PCON  &= ~PCON_PWM

#define enable_ext_int0 TS8052_IE |=  0x01
#define enable_ext_int1 TS8052_IE |=  0x04

#define disable_ext_int0 TS8052_IE &= (~0x01)
#define disable_ext_int1 TS8052_IE &= (~0x04)
#define enable_all_int  TS8052_IE = 0x85
#define disable_all_int TS8052_IE  =  0x05

#define enable_fetch_lba sata.CHP_stripe_h = 0x00
#define disable_fetch_lba sata.CHP_stripe_h = DEF_CHP_STRIPE_H

#define enable_auto_read sata.read_auto_cntl = auto_cmd0_en | auto_cmd1_en
                    
#define disable_auto_read sata.read_auto_cntl  = 0x00;

#define enable_auto_write sata.write_auto_cntl = auto_cmd0_en | auto_cmd1_en 
                    
#define disable_auto_write sata.write_auto_cntl = 0x00;

#define reset_auto_read sata.read_auto_cntl =  sata.read_auto_cntl | auto_reset

#define triger_bus_sel auto_decrease_lba;auto_increase_lba

#define reset_chip SFR_cpu_reboot = 0xb0

#define STRIPE_8K_SET ((DEF_REAL_CHP_STRIPE ) == 0x05)
#define STRIPE_4K_SET ((DEF_REAL_CHP_STRIPE ) == 0x04)
#define STRIPE_2K_SET ((DEF_REAL_CHP_STRIPE ) == 0x03)

///////////////////////////////////////////////////////////////////////////
//(4)global varialbles
///////////////////////////////////////////////////////////////////////////

u8  idata g_current_fis_num;

u8  g_power_mode;

u8 bdata g_flag;
    //(1)LBA check error in ATA command
	sbit g_flag_lba_err 	= g_flag ^ 0;
	//(2)direction of DATA command
	sbit g_flag_dir 	    = g_flag ^ 1;
	//(3)we handle the ncq as continue.
	sbit g_ncq_cont         = g_flag ^ 2;
	//(4)if error happen in ncq, we need to halt command.
	sbit g_ncq_halt         = g_flag ^ 3;
	//(5)fis transmit ok
	sbit g_fis_tx_abort     = g_flag ^ 4;
	//(6) SUP has sent STOP, so no continue 
	sbit g_cont_en          = g_flag ^ 5;
	//(7)chp power save state:
	sbit g_chp_active       = g_flag ^ 6;

    sbit g_enable_write_cache       = g_flag ^ 7;
u8  i;

u8 idata g_tmp_tag;             //for NCQ
u8 idata g_tmp [16];

#ifdef READ_ONLY
u8 read_only_flag;
#endif
u16 idata g_tmp_addr;
//this keeps the sector count
//If in 28 bit and sector count is 8'h0, this is 
u16_t g_sec_cnt;
#define read_code(address) ((unsigned char volatile code *) 0)[address]
#define write_code(address, value) *((u8 xdata *)(address)) = value
#define read_data(address) ((unsigned char volatile xdata *) 0)[address]
#define write_data(address, value) *((u8 xdata *)(address)) = value
							
u8 g_lba[6];

/////////////////////////////////////////////////////////////////////////
//(5)others
/////////////////////////////////////////////////////////////////////////
#define DIR_WRITE 0x00
#define DIR_READ 0x01


#define PIO_WRITE 0x80
#define PIO_READ 0xc0
#define DMA_WRITE 0x90
#define DMA_READ 0xd0

// power mode
#define power_mode_act 0xFF
#define power_mode_standby 0x00
#define power_mode_idle 0x80
#define power_mode_sleep 0x01

#define identify_data_addr 0x7a00

#define power_management_support 0x0008
#define power_management_enable 0x0008


#endif
