#ifndef TYPE_DEF_H
#define TYPE_DEF_H

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int 		BOOL;

typedef union
{ 
	struct
	{
		BYTE	h;	// attention, Low address store H order byte in 8051.  
		BYTE	l;
	} byte;
	WORD word;
} u16_t;

typedef union
{
       WORD     Word[2];
	DWORD 	Number;
	BYTE  	No[4];
} u32_t;

typedef union
{
	WORD 	word[4];
	BYTE  	No[8];
} u64_t;

//just for tempary viariable
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;




#endif

