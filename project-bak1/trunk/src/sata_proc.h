#ifndef SATA_PROC_H
#define SATA_PROC_H




void init();
void reset_engine();
void wait_chp_ready();
void wait_chp_not_ready();


void init_tx_fis();
void tx_fis_34(u8 status, u8 error, u8 flag);
void tx_fis_5f(u8 status, u8 error, u8 flag, u8 seccnt,u8 seccnt_ext, u8 estatus);
void sata_burst(u8 dir, u8 cnt);

void bist_handler();
void int0_handler();
void int1_handler();
void save_lba();
void recover_lba();
void check_power_loss();


#endif
