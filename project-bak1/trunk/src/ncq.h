#ifndef NCQ_H
#define NCQ_H

#ifdef SUPPORT_NCQ


#define check_ncq_read_log (SFR_FIS0_0 == 0x27) && ((SFR_FIS0_1 & 0x80) !=0) && ((SFR_FIS0_2 == ata_read_log_ext) ||(SFR_FIS0_2 == ata_read_log_dma_ext))

#define ncq_abort_all                       0x00
#define ncq_abort_streaming	                0x01
#define ncq_abort_nonstreaming              0x02
#define ncq_abort_selected                  0x03
	
#define queue_error_log                     0x10
#define ncq_management_log                  0x12

//for NCQ
#define NCQ_TABLE_BASE 0x7e40
#define NCQ_MAX_TAG 32
#define NCQ_LINE_LEN 12
#define TAG_FREE 0x00
#define TAG_PENDING 0x01
#define TAG_DONE 0x02

#define ADDR_STATUS 0x00
#define ADDR_DIR 0x01
#define ADDR_SECCNT_LOW 0x02
#define ADDR_LBA0 0x03
#define ADDR_LBA1 0x04
#define ADDR_LBA2 0x05
#define ADDR_LBA3 0x06
#define ADDR_LBA4 0x07
#define ADDR_LBA5 0x08
#define ADDR_SECCNT_HIGH 0x09
#define ADDR_DEVICE 0x0a
#define ADDR_ICC   0x0b

u8      g_ncq_queue;

void tx_fis_41(u8 tag);
void tx_fis_a1(u8 status, u8 error, u8 flag);
void ncq_resp_cmd();
void ncq_data_handler();
void ncq_handler();
void ncq_set_dev_bits();

void ncq_read_log();
void ncq_management_hander();
void abort_ncq();
void read_queue_error_log();
void read_ncq_management_log();


#endif
#endif
