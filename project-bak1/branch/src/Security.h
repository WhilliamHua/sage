

#ifdef SUPPORT_SECURITY

u8 bdata g_security_flag;
     sbit g_security_enable  = g_security_flag^0;
     sbit g_security_lock  = g_security_flag^1;
     sbit g_security_frozen  = g_security_flag^2;
     sbit g_security_HrM  = g_security_flag^3;
     sbit g_security_cnt_expired  = g_security_flag^4;
     sbit g_security_prepared = g_security_flag^5;        
     sbit g_security_pass  = g_security_flag^6;
     sbit g_security_abort  = g_security_flag^7; 

u8 g_TJ_ZSD;//taijixia(1) or zhousidun(0)
//œµÕ≥≈Ã(1),“∆∂Ø≈Ã(0)
u8 g_SYS_YD;

u8 security_mode;
u8 security_state_flag;
u8 security_erase;
u8 uart_show;
u8 support_backup;

u32_t idata g_cap[4];

//#define SEC0    0x00
#define SEC1    0x01
#define SEC2    0x02
//#define SEC3    0x03
#define SEC4    0x04
#define SEC5    0x05
#define SEC6    0x06

#define security_master_log     0x51
#define security_user_log       0x50
#define mbr_encryt_addr         0x52

#define security_cmd_log       0x53/* store user security access information*/

/*offset of security_cmd_log sector*/
#define scl_security_nd_chk   0x01 /*device access need check,whenever has been initialnized*/
#define scl_disk_mode           0x2  /*user partition capacity byte 0*/
#define scl_user_capa0          0x3  /*user partition capacity byte 0*/
#define scl_user_capa1          0x4  /*user partition capacity byte 1*/
#define scl_user_capa2          0x5  /*user partition capacity byte 2*/


#define master_pwd  0x01
#define user_pwd    0x00

#define show_user  0x10
#define show_master 0x20
#define show_all        0x30

#ifdef GPIO_security
#define GPIO_CMD_INACT        0x0F  /*GPIO command is inactive*/
#define GC_TJ_show_master    0x05  /*open taijixia's master partition*/
#define GC_TJ_show_user        0x03  /* open taijixia's user partition*/
#define GC_TJ_erase_user	  0x01 /* erase current user partition */
#define GC_TJ_erase_master    0x08 /* erase current master partition */
#define GC_TJ_erase_all   0x0C  /* erase current all partition */
#define GC_DISK_is_TJ             0x07 /* current configuration is taijixia mode*/
#define GC_ZSD_show_disk       0x02 /*open zhousidun disk */
#define GC_ZSD_erase_disk      0x00 /* erase zhousidun disk*/
#define GC_DISK_is_ZSD		 0x06 /* current configuration is zhousidun mode*/
//#define GC_GPIO_nd_chk		 0x00 /*disk has been initialnized */
#endif

void security_update_id(u16 base_addr,u8 which,u8 arr);
void security_set_pwd();
void security_unlock();
void security_initial();
void analyze_states();
/*
void mbr_burst(u8 cmd);
void unlock_partion();
*/
void choose_lba();
u8 security_cmd_info_read(u8 addr);
void security_cmd_info_write(u8 value, u8 addr);



void check_pwd();


#endif

