Version 1.2.44
2013-07-11
1,Modify recognize disk,which capacity is less than 8G;
2,Increase the time delay:TH58NVG7D2FTA20;
3,Modify a bug of search disk;

Version 1.2.43
2013-07-05
1,Add the configuration refresh to flash page,which get from result of Flash Detect; 
2,Add flash list Die Count,Spare Area Byte;
3,Modify the size of flashList.lib,which can be supported up to 1M;
4,Reduce waiting time;

Version 1.2.42
2013-06-21
1,Support INIC1618L-A25 bridge,modify seal CMD93,94,96 to 92 and 20;
2,Add flash list plane ,extended block,flash readyB5 ;

Version 1.2.41
2013-06-04
1,Modify UART Download Problem;
2,Modify Upgrade change identify data;
3,Modify Download progress£»

Version 1.2.40
2013-05-29
1,Modify SmartData.

Version 1.2.39
2013-04-27
1,Modify Search Device after Download.

version 1.2.38
2013-04-25
1,Modify search UART problem.

version 1.2.37
2013-04-24
1,Modify no flash HW detect problem.
2,Modify KBB address problem.

version 1.2.36
2013-04-15
1,Modfiy search bridge problem.

version 1.2.35
2013-04-11
1,Modfiy tool will clear the download information,if you don't select format.
2,Modfiy BBT error direction.

version 1.2.34
2013-04-02
1,Add original Bad Block info and UID in Smartlog.
2,Add button "Bad Block" to read original Bad Block info.
3,Modify the method of Card Capacity calculation.
4,Move button "KBB" to "SmartInfo",move button "SuperErase" to "HW Information".

version 1.2.33
2013-03-28
1,Add original Bad Block info in SmartData.

version 1.2.32
2013-03-21
1,Modify LogFile,delete UID,merge into BadBlock_Log.
2,Modify SEAL_SUP.v path.

version 1.2.31
2013-03-04
1,Modify New Scan to Scan,quick to scan+ Erase in UART mode.

version 1.2.30
2013-02-28
1,Modify New Scan to Scan,quick to scan+ Erase.
2,Add Original bad block to UID.

version 1.2.29
2013-02-04
1,Vol Det Check in Download.

version 1.2.28
2013-1-28
1,Add SAGE flag in identify device.
2,ADD download percentage.

version 1.2.27
2013-1-24
1,Add Capacity modify by GByte.
2,Delete Sorting.
3,Add Card AES Encryption.

version 1.2.26
2013-1-7
1,Add DSLC in Flash.
2,Modify AddCHP_Setting(Sorting).

version 1.2.25
2012-12-28
1,Modify PSN.

version 1.2.24
2012-12-27
1,Modify Card PSN.
2,Modify Card Format error.
3,Modify Sorting.
4,Add Read SmartData and SuperErase.

version 1.2.23
2012-12-12
1,Modify Read code Version error.
2,Modify Read  Card ID Error.
3.Modify XP Send ATA CMD  Error. 

version 1.2.22
2012-12-10
1,Modify Format error in DOS problem.
2,Add NTFS Format.
3.Add Flash HW detect display how many CEs in one CHP. 

version 1.2.21
2012-12-05
1,Modify Version Display Type.
2,Add MT29F64G08TAAWP(L52A).

version 1.2.20
2012-12-05
1,ADD code Version to identify device for CARD.

version 1.2.19
2012-12-04
1,ADD code Version to identify device.
2,Auto save configure.

version 1.2.18
2012-12-03
1,ADD stripe SUP Setting 0x7FB7.

version 1.2.17
2012-12-03
1,Modify Card capacity error problem.

version 1.2.16
2012-12-03
1,Modify read code version error + 1 (48bit).

version 1.2.15
2012-12-01
1,Modify read code version error.

version 1.2.14
2012-11-30
1,Modify capacity error problem (Greater than 128G).
2,Delete SUP_2K.V for 2K page size.
3,Fix K9WBGZ8U5A Bridge Download bug.
4,ADD HW information detect error red color.

version 1.2.13
2012-11-29
1,Modify capacity error problem (under 8G).
2.ADD Code 1 download 
3,ADD CSmartData.bta
4,Modify HW information button.

version 1.2.12
2012-11-28
1,ADD SSD name to Download page.
2,Modify 2K page stripe.

version 1.2.11
2012-11-28
1,ADD SUP_2K.V for download flash which page size is 2K .
2,ADD cache write and cache read get information in flash list.
3.Delete format add SSD name.
4.ADD CSmartData.bta and FSmartData.bta to SUP code folder for card download Smart and Flash download Smart.

version 1.2.10
2012-11-27
1,ADD K9WBGZ8U5A FLASH type(include 2 channel) DOWNLOAD.
2,Modify flash page flash number to chip number.

version 1.2.08
2012-11-26
1,add Kick Bad block in HW Information.

version 1.2.07
2012-11-23
1,Modify XP format.

version 1.2.06
2012-11-21
1,ADD SCAN MODE AT DOWNLOAD PAGE.

version 1.2.05
2012-11-20
1,Modify auto to Turbo mode at download page.

version 1.2.04
2012-11-20
1,Modify UART Search COM problem.

version 1.2.03
2012-11-20
1,ADD search SSD blue color .

version 1.2.02
2012-11-20
1,FAT32 format support 256G about 4K alignment .
2,ADD LUN to flash list about cache write.

version 1.2.01
2012-11-19
1,FAT32 format support 64G/128G about 4K alignment .
2,Improve card Smart data download.

version 1.2.00
2012-11-17
1,ADD FAT32 format and write /read file 100M/200M test support one port.

version 1.1.98
2012-11-15
1,ADD flashlist.lib check code support 
2,Fix some bug about connect and setting.bta

version 1.1.95
2012-11-09
1,change  percentage Variable;

version 1.1.94
2012-11-09
1,Modify PSN Problem ;

version 1.1.93
2012-11-08
1,Change auto search do not send ForceROM;
2,FIX CARD Download error about time out from 1 to 5;

version 1.1.92
2012-11-08
1,Add card auto detect CHP number;
2,FIX some bug about cuckoo Collapsed;

version 1.1.91
2012-11-07
1,Add default PSN in Download Page;
2,Modify Mode Name;

version 1.1.90
2012-11-06
1,Modify sorting select code about interleave;
2,Add auto search bridge SSD;

version 1.1.89
2012-11-05
1,Modify part number problem;
2,Fix sorting test bug at bridge mode;
3,Fix some bug about turbo mode or capacity percentage;

version 1.1.88
2012-11-02
1,modify search other USB device problem;
2,add download configure to setting for read code version;
3,add sorting function to HW information;

version 1.1.86
2012-10-31
1,ADD flash mode SEAL_CHP.v for HW_DETECT and READ CODE;

version 1.1.85
2012-10-29
1,Default mode,configure error(plane and interleave);
2,Modify USB name problem;
3,ADD flash mode SEAL_CHP.v for HW_DETECT and READ CODE;

version 1.1.83
2012-10-26
1,FIX card bridge download fail problem

version 1.1.82
2012-10-25
1,modify  JumpFlagEnable bit
2,add some flash 

version 1.1.81
2012-10-25
1,ADD interleave
2,ADD bad block detect
3,fix auto detect capacity error

version 1.1.80
2012-10-23
1,ADD bridge download.

version 1.1.70
2012-10-19
1,Support auto detect at Download page.
2,ADD write log.


version 1.1.61
2012-09-29
1.Modify SUP clkset to 0x4B when chance turbo Mode

version 1.1.60
2012-09-29
1.Modify smart data error

version 1.1.50
2012-09-19
1.ADD Download page for manufacture.
2.ADD ReadCodeVersion Button.

version 1.1.44
2012-09-16
1.ADD CODE Version in SUP code (0x7FE0)

version 1.1.42
2012-09-07
1.ADD Turbo mode Select

version 1.1.30
2012-09-04
1.Modify scan bad blocks about which CHP.

version 1.1.20
2012-08-20
1.Add CODE version.

version 1.1.11
2012-08-14
1,Modify Flash CE_num problem.

version 1.1.10
2012-08-14
1,Modify Card capacity check the minimum Card.

version 1.1.02
2012-08-09
1.Modify Check 5Bytes of flash ID.
2.Modify flashlist.lib about K9GBG08U0A flash ID.