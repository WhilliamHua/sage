SET C51INC=C:\Keil\C51\Inc;C:\Keil\C51\Inc\Evatronix
SET C51LIB=C:\Keil\C51\Lib
SET CPU_TYPE=R8051XC (1 DPTR)
SET CPU_VENDOR=Evatronix
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x016E3600
"C:\Keil\C51\BIN\A51.EXE" @STARTUP._ia
"C:\Keil\C51\BIN\C51.EXE" @.\shark.__i
"C:\Keil\C51\BIN\BL51.EXE" @shark.lnp
"C:\Keil\C51\BIN\OH51.EXE" "shark" 
"Z:\trunk - ¸±±¾\proj\Bulid.bat"
