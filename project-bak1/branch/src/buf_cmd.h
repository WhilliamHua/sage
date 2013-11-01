#ifndef BUF_CMD_H
#define BUF_CMD_H

#ifdef BUF_CMD_EN

//our flow:
//1CHP, buf_pkt_cell=0x04, buf_pkt_depth=0x01
//=> buf_pkt_depth=0x10
//=> stripe size == 8K
//=> 5CHP stripe size == 8K
//=> break buffer command
//=> (NCQ write + Buffer command) OR 
//=> NCQ read to improve random read io


//for BUFFER_COMMAND
#define BUF_TABLE_BASE 0x7c00
//should not change this!!!
#define BUF_MAX_DEPTH 0x39




//Big file, break the buffer loop
//0x40: 32K  
//0x20: 16K
//0x08: 4k
#define BUF_SEC_CNT_THRESHOLD 0x08
#define BUF_SEC_CNT_THRESHOLD_1 0x40

#define BUF_LINE_LEN 10
#define BUF_SEL_NONE 0x00


//NOTE: ADDR_BUF_SEL should be 0x00!!!
#define ADDR_BUF_SEL        0x00
#define ADDR_BUF_INDEX      0x01
#define ADDR_BUF_NEW_CMD    0x02
#define ADDR_BUF_NUM        0x03
#define ADDR_BUF_LBA0       0x04
#define ADDR_BUF_LBA1       0x05
#define ADDR_BUF_LBA2       0x06
#define ADDR_BUF_LBA3       0x07
#define ADDR_BUF_LBA4       0x08
#define ADDR_BUF_LBA5       0x09

u8 bdata buf_cmd_flag;

    //new command flag for CHP0 - CHP4
    sbit buf_new_cmd_0           = buf_cmd_flag ^ 0;
    sbit buf_new_cmd_1           = buf_cmd_flag ^ 1;
    sbit buf_new_cmd_2           = buf_cmd_flag ^ 2;
    sbit buf_new_cmd_3           = buf_cmd_flag ^ 3;
    sbit buf_new_cmd_4           = buf_cmd_flag ^ 4;
    sbit buf_new_cmd_5           = buf_cmd_flag ^ 5;
    sbit buf_new_cmd_6           = buf_cmd_flag ^ 6;
    sbit buf_new_cmd_7           = buf_cmd_flag ^ 7;

    
u8 bdata buf_flag;
    sbit buf_data_pending         = buf_flag ^ 0; 
    sbit buf_data_ok                  = buf_flag ^ 1; 
    sbit buf_new_cmd                     = buf_flag ^ 2;
    sbit buf_flag_3                     = buf_flag ^ 3;
    sbit buf_cont_inside_en_next                 = buf_flag ^ 4;
    sbit buf_cont_inside_en       = buf_flag ^ 5; 
    sbit buf_flag_6                      = buf_flag ^ 6;
    sbit buf_flag_7                      = buf_flag ^ 7;

//I will check 
#define BUF_START_CNT 0x04
u8 bdata buf_check_cnt;


//this defines how many sectors in a packet
//0x04: 2K
u8 buf_pkt_cell;
u8 buf_pkt_cell_next;
//this defines how many pakets in the whole 32K FIFO
u8 buf_pkt_depth;
u8 buf_pkt_depth_next;

//ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//buf_pkt_cell * buf_pkt_depth should not > 32K FIFO(total FIFO)
//#if (buf_pkt_cell*buf_pkt_depth > 0x40)
//#error "buf_pkt_cell*buf_pkt_depth > 0x40"
//buf_pkt_cell * buf_pkt_depth*512 byte = 32K byte


u8 buf_wr_ptr;
u8 buf_rd_ptr;
u8 buf_wr_index[MAX_CHP_NUMBER];
u8 buf_rd_index[MAX_CHP_NUMBER];
u16_t buf_sector_num;


#define buf_is_full ((buf_wr_ptr - buf_rd_ptr) == buf_pkt_depth)
#define buf_not_empty (buf_rd_ptr != buf_wr_ptr)



void cmd_buffer_set_param();
void cmd_buffer_init();
void cmd_buffer_write();
void cmd_buffer_read();
void cmd_buffer_check();
u8 cmd_buffer_fetch_bus_sel();



#endif
#endif
