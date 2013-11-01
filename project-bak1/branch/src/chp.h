#ifndef CHP_H
#define CHP_H

#define check_sd_mode ((sata.rst_cntl & 0x80) != 0x00)

//command definition between SUP and CHP
#define CHP_CMD_READ 0x20
#define CHP_CMD_WRITE 0x30
#define CHP_CMD_STOP 0x12
#define CHP_CMD_READ_CAP   0x66
#define CHP_CMD_INIT 0x21
#define CHP_CMD_SHUTDOWN_CLOCK 0xf5
#define CHP_CMD_INT_IDLE 0xf0
#define CHP_CMD_INT_DONE 0xf1
#define CHP_CMD_TRIM  0x50
#define CHP_CMD_FLASH_SLEEP  0x0a
#define CHP_CMD_NEXT_BURST	0x2b
#define CHP_CMD_FLUSH 0x42
#define CHP_CMD_BLOCK 0x70
#define CHP_CMD_ERASE_CNT 0x61
#define CHP_CMD_CARD_CRC 0x65


#define CHP_STATUS_IDLE 0x00
#define CHP_STATUS_PRINT 0xf0

#define CHP_STATUS_HIGH_PRINT_DONE 0x00

void update_chp_status(u8 chp_sel);

void chp_int_handler();
void chp_clock_off();
void chp_clock_on();
void chp_burst(u8 cmd,u8 cnt,u8 base_addr);
void flush_delay();



#endif
