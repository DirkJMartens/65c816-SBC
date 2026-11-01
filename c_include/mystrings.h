#ifndef MYSTRINGS_H
#define MYSTRINGS_H

// for LCD boot screen 
const char LCD_Line1[16] = "WDC65C816-based";
const char LCD_Line2[16] = "computer by DMA";

// for console boot screen 
const char MSG_TERM_WELC[] =
		"   __ _____  ___  __   __\r\n"						// 25+2 chars
		"  / /| ____|/ _ \\/_ | / /\r\n"					// +25+2=54 chars
		" / /_| |__ | (_) || |/ /_\r\n"						// +25+2=81 chars
		"|  _ \\___ \\ > _ < | |  _ \\\r\n"					// +26+2=109 chars
		"| (_) |__) | (_) || | (_) |\r\n"					// +27+2=138 chars
		" \\___/____/ \\___/ |_|\\___/ \r\n\r\n"			// +27+4=169 chars 
		"65c816 BIOS (16 bit NATIVE MODE)\r\n"				// +32+2=203 chars 
		"H/W by WDC - S/W by BCS\r\n"						// +23+2=228 chars
		"(c) DMA 2024 - ? for help\r\n";					// +25+2=255 chars

// for command interpreter 
					   //0000000000111111111122222222223333333333444444444455555555556
					   //0123456789012345678901234567890123456789012345678901234567890
const char record[80] = ":1856200020A9208DE07FC220E220A5012908C220F00382B4FFE220A9DB"; 	
char startcode[2]	= ""; 	//:
char bytecount[3]	= ""; 	//18
char address[5]		= ""; 	//5620
char rectype[3]		= "";	//00 or //01 
char payload[49]	= "";	//20A9208DE07FC220E220A5012908C220F00382B4FFE220A9
char checksum[3]	= ""; 	//DB

// Help menu 
const char HELP_MENU[] = 
		"? - Display this menu\r\n"
		// "A - Assemble\r\n"
		// "B - Run MSBasic\r\n"
		// "C - Compare memory\r\n"
		// "D - Disassemble\r\n"
		"F - Fill memory (F start end fill)\r\n"
		// "G - Go\r\n"
		// "H - Hunt\r\n"
		// "I - \r\n"
		// "J - Jump to subroutine\r\n"
		// "K - \r\n"
		"L - Load program\r\n"
		"M - Display memory (M start end)\r\n"
		// "N - Numeric conversion\r\n"
		// "O - \r\n"
		// "P - \r\n"
		// "Q - \r\n"
		"R - Display registers\r\n"
		// "S - Run POST self-test diagnostics\r\n"
		// "T - Transfer memory\r\n"
		"U - Update date/time\r\n"
		// "V - \r\n"
		"W - Run WozMon\r\n"
		// "X - Exit to BIOS\r\n"
		// "Y - \r\n"
		// "Z - \r\n"
		// " - \r\n"
		// "; - Change registers\r\n\0"
		": - Change memory(: mem val1 [val2 ...])\r\n\0";

// string constants, stored in KDATA memory section 
// Info messages 
const char ASSEMBLE_MSG[] = "Assemble Code";			// Assemble 
const char ATADDR_MSG[] = " at starting address 0x";
const char BCK_GO[] = "Back from subroutine";
const char CHNGMEMSYNTAX_MSG[] = "Syntax: \"> xxxx yy\" with xxxx = 0000 .. FFFF and yy = 00 .. FF"; 
const char CHNGREG_MSG[] = "Change Register"; 
const char COMPMEM_MSG[] = "Compare Memory";			// 
const char DISASSEM_MSG[] = "Disassemble Code";		// 
const char DISPMEM_MSG[] = "Displaying Content of Memory Range 0x"; 
const char DISPMEMSYNTAX_MSG[] = "Syntax: \"M xxxx [yyyy]\" with xxxx/yyyy = 0000 .. FFFF"; 
const char DISPREG_MSG[] = "Display Register Content"; 
const char DONE_MSG[] = "Command completed"; 
const char DOTS_MSG[] = " ... done"; 
const char EXIT_MSG[] = "Exit To Bios"; 
const char FILLMEM_MSG[] = "Filling Memory Range";			// 
const char FROM_MSG[] = " from 0x"; 
const char GO_MSG[] = "Go Memory"; 
const char HUNT_MSG[] = "Hunt Memory"; 
const char JUMP_MSG[] = "Jump To Sub"; 
const char LOAD_PRG[] = "Loading program"; 
const char LOCATION_MSG[] = " in memory location 0x"; 
const char MEMHDR[80] = "BA Addr | 00 01 02 03 04 05 06 07 - 08 09 0A 0B 0C 0D 0E 0F | ASCII"; 
const char MEMSEP[80] = "--------+---------------------------------------------------+-----------------";
const char MSBASIC_MSG[] = "MS BASIC - (c) 2024";	// MS-BASIC Banner 
const char NUMCONV_MSG[] = "Numeric Conversion : N <radix><value> with <radix>=[0x=HEX]/[-=DEC]/[0O=OCT]/[0b=BIN]"; 
const char REGHDR[80] = "  PB  PC   NVmxDIZC  .C   .X   .Y   SP   DP  DB"; 
const char SELFTEST_MSG[] = "Self-test diagnotics";
const char STORED_MSG[] = "Storing value 0x"; 
const char STORING_MSG[] = "Storing ";
const char TO_MSG[] = " to 0x"; 
const char UPDTIME_MSG[] = "Update system date and/or time"; 
const char UPDTIMESYNTAX_MSG[] = "Syntax: U HH:MM:SS D MM/DD/YY"; 
const char TRANSMEM_MSG[] = "Transfer Memory"; 
const char VALUES_MSG[] = " values ";
const char WITH_MSG[] = " with value 0x"; 
// Error messages 
const char INVADDR_ERR[] = "Invalid address format, max 4 chars, 0..9 and a/A..f/F"; 
const char INVVAL_ERR[] = "Invalid value format, max 2 chars, 0..9 and a/A..f/F"; 
const char ILLADD_ERR1[] = "Address not a hex number from 0000 to FFFF"; 
const char ILLADD_ERR2[] = "Address contains non-hex digits"; 
const char MISVAL_ERR[] = "Command requires at least 1 value"; 
const char SYNTAX_ERR[] = "Command syntax error"; 
const char UNKNOWN_ERR[] = "Unknown command"; 

#endif 				// end MYSTRINGS_H