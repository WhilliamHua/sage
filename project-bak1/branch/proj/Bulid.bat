call keil_build.bat


del *.img
del *.upd


 rem ..\util\hex2bin /L32766  shark.hex shark.upd
..\util\hex2bin-new  -l 8000  -e upd shark.hex

copy shark.upd RAM.UPD
..\util\bin2txt.exe 


copy RAM-DW.IMG RAM.v
 ..\util\add_setting.exe
copy RAM-ADD-SETTING.v  ram8051nocrc.v



rem insert identify data

 ..\util\write_identify_data.exe
 copy ram8051_after_ident.v ram8051nocrc.v


..\util\HexGoSUPv1.26


copy ram8051.v SUP.v
copy SUP.v ..\..\cuckoo\CODE\SUP\SUP.v
copy SUP.v ..\..\SUP_release\SUP.v
copy SUP.v ..\..\..\SUP_release\SUP.v
copy SUP.v F:\work\S685\design\tb\bench\SUP_ROM.rcf
copy SUP.v ..\..\..\MPTool_Release\Cuckoo\CODE\SUP\SUP.v

del shark.hex shark.upd ram8051.bta ram8051.v ram8051_after_ident.v ram8051nocrc.v  RAM.UPD  RAM-BYTE.IMG RAM-DW.IMG RAM-ADD-SETTING.v S681_RAM.bta
pause
exit
