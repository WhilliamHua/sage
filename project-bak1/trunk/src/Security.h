

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



u8 security_mode;
u8 security_state_flag;

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


#define master_pwd  0x01
#define user_pwd    0x00

#define show_user  0x10
#define show_master 0x20

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




void check_pwd();


#endif

