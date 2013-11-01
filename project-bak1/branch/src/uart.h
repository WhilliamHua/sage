#ifndef UART_H
#define UART_H

#define wait_uart_tx_done while((sata.uart_status & 0x08) == 0x00)

#define UART_DWLD_CMD							0x27
#define UART_DWLD_MICRO							0x92
#define UART_DWLD_READ							0x20
#define UART_W_BUF_SETUP						0x30
#define UART_W_BUF_DATA							0x31
#define UART_R_BUF_SETUP						0x20
#define UART_WRITE_REG_BYTE						0x51
#define UART_READ_REG_BYTE 						0x50
#define UART_WRITE_REG_WORD						0x53
#define UART_CONTROL_CMD                                         0x55


#ifdef ENABLE_PRINT

#pragma disable
void uart_print_byte(u8 tx_data);

#pragma disable
void uart_tx_byte(u8 tx_data);

#pragma disable
void printu(const u8 * fmt,...);

#endif

#ifdef ENABLE_UART_INT
#pragma disable  
void uart_send_R1(u8 tx_data);
void uart_int_handler();
#endif


#endif
