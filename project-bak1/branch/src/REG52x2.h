/*-------------------------------------------------------------------------
REG52x2.H

Header file for AtmelWM TS80C52x2/54x2/58x2
  
Copyright (c) 1988-2002 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#ifndef __REG52x2_H__
#define __REG52x2_H__

sfr  TS8052_P0      =   0x80;			 

sbit  TS8052_P0_7    =   TS8052_P0^7;
sbit  TS8052_P0_6    =   TS8052_P0^6;
sbit  TS8052_P0_5    =   TS8052_P0^5;
sbit  TS8052_P0_4    =   TS8052_P0^4;
sbit  TS8052_P0_3    =   TS8052_P0^3;
sbit  TS8052_P0_2    =   TS8052_P0^2;
sbit  TS8052_P0_1    =   TS8052_P0^1;
sbit  TS8052_P0_0    =   TS8052_P0^0;

sfr  TS8052_SP      =   0x81;
sfr  TS8052_DPL     =   0x82;
sfr  TS8052_DPH     =   0x83;

sfr  TS8052_PCON    =   0x87;			 

#define	PCON_IDLE		0x01	
#define	PCON_STOP		0x02	
#define	PCON_HSKEN	    0x04	
#define	PCON_P2Sel		0x08	
#define	PCON_PWM		0x10	
#define	PCON_ISRT		0x20	
#define	PCON_WDTT		0x40	
#define	PCON_SMOD		0x80	


sfr  TS8052_TCON    =   0x88;             

sbit  TS8052_TF1    =   TS8052_TCON^7;
sbit  TS8052_TR1    =   TS8052_TCON^6;
sbit  TS8052_TF0    =   TS8052_TCON^5;
sbit  TS8052_TR0    =   TS8052_TCON^4;
sbit  TS8052_IE1_   =   TS8052_TCON^3;
sbit  TS8052_IT1    =   TS8052_TCON^2;
sbit  TS8052_IE0_   =   TS8052_TCON^1;
sbit  TS8052_IT0    =   TS8052_TCON^0;

sfr  TS8052_TMOD    =   0x89;			 

sfr  TS8052_TL0     =   0x8A;			 

sfr  TS8052_TL1     =   0x8B;			 

sfr  TS8052_TH0     =   0x8C;			 

sfr  TS8052_TH1     =   0x8D;			 

sfr  TS8052_P1      =   0x90;			 

sbit  TS8052_P1_7    =   TS8052_P1^7;
sbit  TS8052_P1_6    =   TS8052_P1^6;
sbit  TS8052_P1_5    =   TS8052_P1^5;
sbit  TS8052_P1_4    =   TS8052_P1^4;
sbit  TS8052_P1_3    =   TS8052_P1^3;
sbit  TS8052_P1_2    =   TS8052_P1^2;
sbit  TS8052_P1_1    =   TS8052_P1^1;
sbit  TS8052_P1_0    =   TS8052_P1^0;

sbit  TS8052_T2EX    =   TS8052_P1^1;
sbit  TS8052_T2      =   TS8052_P1^0;

sfr  TS8052_SCON    =   0x98;			 

sbit  TS8052_SM0    =   TS8052_SCON^7;
sbit  TS8052_FE     =   TS8052_SCON^7;
sbit  TS8052_SM1    =   TS8052_SCON^6;
sbit  TS8052_SM2    =   TS8052_SCON^5;
sbit  TS8052_REN    =   TS8052_SCON^4;
sbit  TS8052_TB8    =   TS8052_SCON^3;
sbit  TS8052_RB8    =   TS8052_SCON^2;
sbit  TS8052_TI     =   TS8052_SCON^1;
sbit  TS8052_RI     =   TS8052_SCON^0;

sfr  TS8052_SBUF    =   0x99;			 

sfr  TS8052_P2      =   0xA0;			 

sbit  TS8052_P2_7    =   TS8052_P2^7;
sbit  TS8052_P2_6    =   TS8052_P2^6;
sbit  TS8052_P2_5    =   TS8052_P2^5;
sbit  TS8052_P2_4    =   TS8052_P2^4;
sbit  TS8052_P2_3    =   TS8052_P2^3;
sbit  TS8052_P2_2    =   TS8052_P2^2;
sbit  TS8052_P2_1    =   TS8052_P2^1;
sbit  TS8052_P2_0    =   TS8052_P2^0;

sfr  TS8052_IE     =   0xA8;			     
sfr  TS8052_IE0    =   0xA8;

sbit  TS8052_EA     =   TS8052_IE^7;
sbit  TS8052_ET2    =   TS8052_IE^5;
sbit  TS8052_ES     =   TS8052_IE^4;
sbit  TS8052_ET1    =   TS8052_IE^3;
sbit  TS8052_EX1    =   TS8052_IE^2;
sbit  TS8052_ET0    =   TS8052_IE^1;
sbit  TS8052_EX0    =   TS8052_IE^0;

sfr  TS8052_SADDR   =   0xA9;			 

sfr  TS8052_P3      =   0xB0;			 

sbit  TS8052_P3_7    =   TS8052_P3^7;
sbit  TS8052_P3_6    =   TS8052_P3^6;
sbit  TS8052_P3_5    =   TS8052_P3^5;
sbit  TS8052_P3_4    =   TS8052_P3^4;
sbit  TS8052_P3_3    =   TS8052_P3^3;
sbit  TS8052_P3_2    =   TS8052_P3^2;
sbit  TS8052_P3_1    =   TS8052_P3^1;
sbit  TS8052_P3_0    =   TS8052_P3^0;

sbit  TS8052_RD      =   TS8052_P3^7;
sbit  TS8052_WR      =   TS8052_P3^6;
sbit  TS8052_T1      =   TS8052_P3^5;
sbit  TS8052_T0      =   TS8052_P3^4;
sbit  TS8052_INT1    =   TS8052_P3^3;
sbit  TS8052_INT0    =   TS8052_P3^2;
sbit  TS8052_TXD     =   TS8052_P3^1;
sbit  TS8052_RXD     =   TS8052_P3^0;

sfr  TS8052_IPH    =   0xB7;			     
sfr  TS8052_IPH0    =   0xB7;


sfr  TS8052_SADEN   =   0xB9;			 

sfr  TS8052_IPL    =   0xB8;			     
sfr  TS8052_IPL0    =   0xB8;	

sbit  TS8052_IPLT2    =   TS8052_IPL^5;
sbit  TS8052_IPLS     =   TS8052_IPL^4;
sbit  TS8052_IPLT1    =   TS8052_IPL^3;
sbit  TS8052_IPLX1    =   TS8052_IPL^2;
sbit  TS8052_IPLT0    =   TS8052_IPL^1;
sbit  TS8052_IPLX0    =   TS8052_IPL^0;

sfr  TS8052_T2CON   =   0xC8;             

sbit  TS8052_TF2    =   TS8052_T2CON^7;
sbit  TS8052_EXF2   =   TS8052_T2CON^6;
sbit  TS8052_RCLK   =   TS8052_T2CON^5;
sbit  TS8052_TCLK   =   TS8052_T2CON^4;
sbit  TS8052_EXEN2  =   TS8052_T2CON^3;
sbit  TS8052_TR2    =   TS8052_T2CON^2;
sbit  TS8052_CT2    =   TS8052_T2CON^1;
sbit  TS8052_CPRL2  =   TS8052_T2CON^0;

sfr  TS8052_T2MOD   =   0xC9;			 

sfr  TS8052_RCAP2L  =   0xCA;			 
sfr  TS8052_RCAP2H  =   0xCB;			 
sfr  TS8052_TL2     =   0xCC;			 
sfr  TS8052_TH2     =   0xCD;			 

sfr  TS8052_PSW     =   0xD0;			 
sbit  TS8052_CY     =   TS8052_PSW^7;
sbit  TS8052_AC     =   TS8052_PSW^6;
sbit  TS8052_F0     =   TS8052_PSW^5;
sbit  TS8052_RS1    =   TS8052_PSW^4;
sbit  TS8052_RS0    =   TS8052_PSW^3;
sbit  TS8052_OV     =   TS8052_PSW^2;
sbit  TS8052_UD     =   TS8052_PSW^1;
sbit  TS8052_P      =   TS8052_PSW^0;

sfr  TS8052_ACC     =   0xE0;			 
sfr  TS8052_B       =   0xF0;			 

sfr  TS8052_CKCON      =   0x8E;			 

#endif


