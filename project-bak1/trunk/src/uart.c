#ifdef ENABLE_PRINT


#pragma disable
void printu(const char* fmt, ...)
{
   u8 idata d;
   u8 idata ch=0;
   va_list ap;
   const u8 idata * s;
   //disable_ext_int1;
  disable_all_int;
   
   va_start(ap,fmt);       //this macro is in stdarg.hÐ
   while (*fmt)
       {
           if (*fmt != '%')
           {
               uart_tx_byte(*fmt++);                
               continue; 
           }
           switch (*++fmt)
           {
               case 's':                
               s = va_arg(ap,const u8 *);//get the next argument
               for (; *s; s++)
               uart_tx_byte(*s);
               break;
               case 'x':
               d = va_arg(ap,u8);
               uart_print_byte(d);
               break;
               
               case 'c':
               ch = va_arg(ap,u8);
               uart_tx_byte(ch);
               break;
               
               default:
               uart_tx_byte(*fmt);
               break;
           }
           fmt++;
       }
   va_end(ap);
   delay(200);    
   //enable_ext_int1
   enable_all_int;
}


#pragma disable
void uart_tx_byte(u8 tx_data)
{
    
    //disable_ext_int1;
    disable_all_int;
	sata.uart_tx_data = tx_data;
	wait_uart_tx_done;
	delay(200);
	//enable_ext_int1;
	enable_all_int;
	
}

#pragma disable
void uart_print_byte(u8 tx_data)
{
    
    u8 tmp_data;
    //disable_ext_int1;
    disable_all_int;
    tmp_data = tx_data >> 4;
    if(tmp_data>0x09)tmp_data+=0x37;
    else tmp_data += 0x30; //+tmp_data;
	sata.uart_tx_data = tmp_data;
    wait_uart_tx_done;
    delay(200);

	tmp_data = tx_data & 0x0f;
	if(tmp_data>0x09)tmp_data+=0x37;
    else tmp_data += 0x30; //+tmp_data;
	sata.uart_tx_data = tmp_data;
	wait_uart_tx_done;
	delay(200);
	//enable_ext_int1;
	enable_all_int;
	
}




#endif  //ifdef ENABLE PRINT



#ifdef ENABLE_UART_INT

#pragma disable
void uart_send_R1(u8 tx_data)
{
   sata.uart_tx_data = 0x55;
   sata.uart_tx_data = tx_data;
   wait_uart_tx_done; 
}

/////////////////////////////////////////////////////////////////
//To WRITE or READ SATA external registers(0xffxx)
/////////////////////////////////////////////////////////////////
void uart_int_handler()
{
    u8 uart_cmd_byte, uart_b3,uart_b2,uart_b1,uart_b0;
    u16_t reg_addr;
    
   	uart_cmd_byte = sata.uart_rx_data;
	sata.uart_rx_data = 0;	 // increase uart rx fifo read address
	uart_b3 = sata.uart_rx_data;
	sata.uart_rx_data = 0;	 // increase uart rx fifo read address
	uart_b2 = sata.uart_rx_data;
	sata.uart_rx_data = 0;	 // increase uart rx fifo read address
	uart_b1 = sata.uart_rx_data;
	sata.uart_rx_data = 0;	 // increase uart rx fifo read address
	uart_b0 = sata.uart_rx_data;
    sata.uart_rx_stat1 |= 0x80; //reset the rx fifo

    reg_addr.byte.l = uart_b2;
    reg_addr.byte.h = uart_b3;
    
    if(uart_cmd_byte == UART_WRITE_REG_BYTE)
    {
        //write register 0xffxx
        write_data(reg_addr.word,uart_b0);
        uart_send_R1(uart_b0);
    }
	    
	else if(uart_cmd_byte == UART_READ_REG_BYTE)
	{
	    //read register 0xffxx
	    uart_b0 =  read_data(reg_addr.word);
        uart_send_R1(uart_b0);
	}
	
    sata.uart_int = sata.uart_int;
    
}
#endif


