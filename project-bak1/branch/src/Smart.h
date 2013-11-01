

#ifdef SUPPORT_SMART



u8 bdata g_smart_flag;
     sbit g_smart_operation  = g_smart_flag^0;
     sbit g_smart_attribute_autosave  = g_smart_flag^1;
     sbit g_smart_offline_flag  = g_smart_flag^2;
     sbit g_smart_read_flag  = g_smart_flag^3;
     sbit g_smart_log_find  = g_smart_flag^4;
     sbit g_smart_test_int  = g_smart_flag^5;        
     sbit g_smart_flag_6  = g_smart_flag^6;
     sbit g_smart_abort  = g_smart_flag^7;        


u8 bdata g_smart_handle_flag;
	sbit g_smart_nfirst_handle = g_smart_handle_flag^0;
	sbit g_smart_read_handle = g_smart_handle_flag^1;
	sbit g_smart_write_handle = g_smart_handle_flag^2;
	sbit g_smart_handle_flag_3 = g_smart_handle_flag^3;
	sbit g_smart_handle_flag_4 = g_smart_handle_flag^4;
	sbit g_smart_handle_flag_5 = g_smart_handle_flag^5;
	sbit g_smart_handle_flag_6 = g_smart_handle_flag^6;
	sbit g_smart_power_down_flag = g_smart_handle_flag^7;


u8 idata g_smart_block_cnt_l;
u8 idata g_smart_block_cnt_h;

u8 idata g_smart_write_cnt;
u8 idata g_smart_read_cnt;
u16 idata write_sec_cnt_l;
u8 idata write_sec_cnt_h;
u16 idata read_sec_cnt_l;
u8 idata read_sec_cnt_h;

//smart infor addr
#define smart_power_num_addr    0x07    //0c
#define smart_phy_down_addr     0x13    //ad
#define smart_power_down_addr   0x1f    //ae
#define smart_softrst_num_addr  0x2b    //bc
#define smart_temperature       0x37    //c2
#define smart_block_error_addr  0x43    //c3
#define smart_ecc_num_addr      0x4f    //c4
#define smart_crc_num_addr      0x5b    //c7
#define smart_left_time         0x65    //e7
#define smart_write_addr        0x73    //f1
#define smart_read_addr         0x7f    //f2
#define smart_erase_addr        0x8b    //f3
#define smart_program_fail      0x97    //ab
#define smart_block_fail        0xa3    //ac
#define smart_card_infor3       0xaf    //f6
#define smart_card_infor4       0xbb    //f7
#define smart_card_crc_addr0    0xc7    //fa
#define smart_card_crc_addr1    0xd3    //fb
#define smart_card_crc_addr2    0xdf    //fc
#define smart_card_crc_addr3    0xeb    //fd
#define smart_card_crc_addr4    0xf7    //fe

#define smart_100               0x64

//u8 idata g_smart_log_find;
u8 idata g_smart_test_status;


//SMART SUBCOMMAND
#define smart_read_data                     0xd0
#define smart_read_threshold             0xd1
#define smart_attribute_autosave       0xd2
#define smart_execute_offline             0xd4
#define smart_read_log                        0xd5
#define smart_enable_operation          0xd8
#define smart_disable_operation         0xd9
#define smart_return_status                0xda
#define smart_write_log                       0xd6

//smart read log log address define
#define log_directory                                 0x00
#define summary_error_log                      0x01
#define comprehensive_error_log             0x02
#define self_test_log                                  0x06
#define selective_self_text_log                   0x09
#define block_base_log                             0x10

//smart structure
#define smart_attribute     0x00
#define smart_threshold    0x01

#define smart_log_directory                                 0x03
#define smart_summary_error_log                      0x04
#define smart_comprehensive_error_log             0x05
#define smart_comprehensive_error_log_ext     0x06
#define smart_self_test_log                                  0x07
#define smart_self_test_log_ext                           0x08
#define smart_selective_self_text_log                   0x09
#define smart_streaming_performance_log         0x10
#define smart_write_stream_error_log                0x11
#define smart_read_stream_error_log                     0x12
#define smart_delayed_sector_log                        0x13


//smart_attribute_autosave
#define smart_attribute_autosave_dis    0x00
#define smart_attribute_autosave_en    0xf1


//off_line subcommand address define
#define offline_routine 					0x00
#define short_self_test_offline				0x01
#define self_test_offline_ext				0x02
#define conveyance_self_test_offline		0x03
#define selective_self_test_offline			0x04

#define abort_self_test_offline				0x7f

#define short_self_test_captive				0x81
#define self_test_captive_ext				0x82
#define conveyance_self_test_captive		0x83
#define selective_self_test_captive			0x84


//smart_format
#define smart_attribute_value_base		0x0005
#define smart_threshold_value_base		0x0203
#define smart_attribute_format_lenght	0x12

#define smart_attribute_number_max		0x1e
#define smart_attribute_number			0x10

void smart_handler();
void smart_offline();
void smart_data_handler(u8 base_sec,u8 sec_cnt,u8 chp_cmd);
void smart_log(u8 chp_cmd);
void smart_status();
//void smart_compare_attribute();
void self_test_routine();


//CHP

//smart handle
void smart_card_update();
void updata_smart(u8 smart_addr, u8 smart_cnt);
void updata_smart_n(u8 smart_addr, u8 smart_cnt);
void smart_write_cnt();
void smart_read_cnt();
void smart_block_handle();
void smart_erase_handle();
void smart_card_crc();


#endif 