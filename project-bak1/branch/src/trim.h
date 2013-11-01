#ifdef SUPPORT_TRIM

#define lba_continue 0x00;
#define lba_discontinue 0x01


u8 bdata g_trim_flag;
     sbit g_trim_burst_abort  = g_trim_flag^0;
     sbit g_trim_flag_1  = g_trim_flag^1;
     sbit g_trim_flag_2  = g_trim_flag^2;
     sbit g_trim_flag_3  = g_trim_flag^3;
     sbit g_trim_flag_4  = g_trim_flag^4;
     sbit g_trim_flag_5  = g_trim_flag^5;        
     sbit g_trim_flag_6  = g_trim_flag^6;
     sbit g_trim_flag_7  = g_trim_flag^7; 
     
u32_t idata trim_addr_offset;
u64_t idata start_address;
u32_t idata sum;
u8 idata lbaa[6];

void trim_handler();
void fetch_block_border(u16_t addr);
void lba_adder();
u8 cmp_lba();
void send_trim_cmd(u32_t sec_cnt);
void trim_burst(u32_t block_num);
void lba_shift_right();
void fetch_table_lba();
void compute_table_base(u8 offset);
u8 trim_check_lba();
#endif //#ifdef SUPPORT_TRIM