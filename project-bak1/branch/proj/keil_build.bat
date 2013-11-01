SET C51INC=C:\Keil905\C51\INC\Evatronix\;C\Keil905\C51\INC\
SET C51LIB=C:\Keil905\C51\LIB
SET CPU_TYPE=R8051XC (1 DPTR)
SET CPU_VENDOR=Evatronix
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x016E3600

rem if use dw8051
rem SET C51INC=C:\Keil905\C51\INC\Dallas\;C:\Keil905\C51\INC\
rem SET C51LIB=C:\Keil905\C51\LIB
rem SET CPU_TYPE=DW-8051
rem SET CPU_VENDOR=Synopsys
rem SET UV2_TARGET=Target 1
rem SET CPU_XTAL=0x00B71B00

"C:\Keil905\C51\BIN\A51.EXE" @STARTUP._ia


"C:\Keil905\C51\BIN\C51.EXE" @.\shark.__i
 
color 79
echo *******************************************
"C:\Keil905\C51\BIN\BL51.EXE" @shark.lnp
echo *******************************************
pause
color 07
"C:\Keil905\C51\BIN\OH51.EXE" "shark" 
