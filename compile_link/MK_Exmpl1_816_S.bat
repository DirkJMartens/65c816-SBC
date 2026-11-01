cls
@echo off
setlocal enableextensions
set "OUTFILES=C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\output_files"
set "ASMFILES=C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\asm"
set "C_FILES=C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_src"
set "OPT_FILES=C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\compile_link"
rem @echo on
del  /q %OUTFILES%\WDC_CStartup_816.obj
del  /q %OUTFILES%\C_TEMPLATE_816.obj
del  /q %OUTFILES%\wozmon.obj
del  /q %OUTFILES%\WDC_CStartup_816.lst
del  /q %OUTFILES%\C_TEMPLATE_816.lst
del  /q %OUTFILES%\wozmon.lst
REM del WDC_CStartup_816.sym
del  /q %OUTFILES%\C_TEMPLATE_816.sym
del  /q %OUTFILES%\C_TEMPLATE_816.map
rem goto :end
REM del /q output_files\*.tmp
REM -G = assemble source level info (essential so .bin file can be used with WDCDB) 
REM -L = generate .LST file 
REM -V = display memory usage 
REM -W = page width = 132 chars 
REM -DUSING_816 = define global equate "USING_816"
REM 	used by compiler to ensure 816 instruction set is used 
REM -DSMALL = define global equate "SMALL" 
REM 	used by compiler/linker to ensure "SMALL" memory model is used 
REM		SMALL = use 16 bit pointers for code and data (both <=64K) 
REM 	MEDIUM = use 16 bit pointers for data and 32 bit pointers for code 
REM		COMPACT = use 16 bit pointers for code and 32 bit pointers for data 
REM		LARGE = use 32 bit pointers for code and data (both >64K) 
REM
@echo on 
WDC816AS -G -L -W -V -DUSING_816 -DSMALL -o %OUTFILES%\WDC_CStartup_816.obj %ASMFILES%\WDC_CStartup_816.ASM
rem WDC816AS -G -L -W -V -DUSING_816 -DSMALL -o %OUTFILES%\wozmon.obj wozmon.ASM
@echo off
REM -BS : turn on source file debugging 
REM -DUSING_816 : see above 
REM -LT : generate listing with embedded source statements  
REM -LW = generate wide format .LST file 
REM -MS : generate code for the small memory model 
REM -MV : places string data in the STRING section i.o default KDATA
REM -MU : places all CONST defined variables in KDATA i.o. default DATA 
REM -QP : generate a .PRO file with prototypes for all the non-static functions
REM 		do not use as it will delete .obj files 
REM -PA : turn on all the ANSI checking and turn off any special extensions
REM 		do not use when interrupt functions are used 
REM -PX : allow C++ style comments 
REM -SI : assume all array indexes will always be positive (so X/Y regs will be used) 
REM -WP : warning if a header file is not being included
REM -WW : continue compiler beyond 5 errors
@echo on 
WDC816CC -BS -DUSING_816 -LT -LW -MSUV -SI -PX -WP -o %OUTFILES%\C_TEMPLATE_816.obj %C_FILES%\C_TEMPLATE_816.c 
@echo off
rem WDC816CC -BS -DUSING_816 -LT -LW -MS -MU -PX -SI -WW cmdmon.c 
REM -AINPUT_BUFFER=1000 : specifies location of keyboard / console input buffer
REM -ABUFFER_IDX=1100 : specifies location of circular buffer pointers 
REM -D2000 : specifies location of DATA (initialized but changing)
REM -ASHADOW=7EE0 : specifies location of interrupt vectors in "SHADOW: section"
REM -C8000 : specifies location of actual CODE 
REM -KF000 : specifies location of initalized, constant data (e.g. constant text strings) 
REM -ASTARTUP=FF00 : specifies location of interrupt vectors in "STARTUP: section"
REM -AVECTORS=FFFC : specifies location of interrupt vectors in "VECTORS: section" 
REM -SZ : use WDC symbol format (essential for wdcdb)
REM -HZ : use WDC HEX output format (essential for wdcdb)
REM -HI : Intel HEX format 
REM -PFF : fill char for empty space in hex file 
REM -HB : use straight binary format (typical for burning ROM) 
REM -G : generate source debug information 
REM -T : generate a .MAP file 
REM -LCS : use library "LCS.LIB" 
REM 	valid libs for 65C02: c.lib or m.lib 
REM 	valid libs for 65C816: 
REM 		cs.lib, cc.lib, cm.lib, cl.lib, 	// all standard ANSI C functs (sml/comp/med/lrg)
REM 		coc.lib, com.lib, col.lib			// ??  (sml/comp/med/lrg)
REM 		ms.lib, mc.lib, mm.lib, ml.lib, 	// floating point math library funcs (sml/comp/med/lrg)
REM -O : specifies name of output file
REM WDCLN -AINPUT_BUFFER=1000 -D2000 -ASHADOW=7EE0 -C8000 -KF800 -ASTARTUP=FF00 ^
rem WDCLN -F bin_file.opt output_files\WOZMON.OBJ output_files\WDC_CStartup_816.obj output_files\C_TEMPLATE_816.obj -lcs -o output_files\C_TEMPLATE_816.bin
WDCLN -F %OPT_FILES%\bin_file.opt %OUTFILES%\WDC_CStartup_816.obj %OUTFILES%\C_TEMPLATE_816.obj -lcs -o %OUTFILES%\C_TEMPLATE_816.bin
rem WDCLN -F hex_file.opt output_files\WOZMON.OBJ output_files\WDC_CStartup_816.obj output_files\C_TEMPLATE_816.obj -lcs -o output_files\C_TEMPLATE_816.hex
@echo on
WDCLN -F %OPT_FILES%\hex_file.opt %OUTFILES%\WDC_CStartup_816.obj %OUTFILES%\C_TEMPLATE_816.obj -lcs -o %OUTFILES%\C_TEMPLATE_816.hex
@echo off 
rem python disasm65816.py -f 1 -u C_TEMPLATE_816.bin > C_TEMPLATE_816.dis
REM ***** WDCLN -c1000 -sz -hz -g -t t0s.obj exmpl1.obj -lcs -o exmpl1.bin 

REM allows to examine the .obj files created by the assembler 
rem @echo on
rem @echo off
rem WDCOBJ C_TEMPLATE_816 > objects_816S.txt

REM allows to examine the .SYM files created by the linker 
rem @echo on
rem @echo off
rem WDCSYM C_TEMPLATE_816 > symbols_816S.txt

REM ALL Done!

pause	
:end
endlocal 
