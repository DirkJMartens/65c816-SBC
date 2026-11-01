#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\cmdmon.h" 
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\ACIA.h"
#include <string.h> 			// for strtok, strcpy, strcat, strncpy functions
#include <ctype.h> 				// for isprint(), isxdigit() functions
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\RTC.h"				// for updating date/time 

void parse_cmd(char *s) {
	// Functions that splits a given command string into its components 
	// INPUT: s is a pointer to the command string 
	// OUTPUT: 
	//		cmd : letter indicating the command (e.g. M, F, ?, ...) 
	// 		operand1..3 : parameters given with the command (e.g. start/end addr, mem value..) 
    char *token = NULL; 		// 
	char cmd = '\0'; 			// 
	const char delim[] = " ";	// 

	operand1[0]='\0';
	operand2[0]='\0';
	operand3[0]='\0';

	token = strtok(s, delim);		// parse the 1st token 
	if (token!=NULL) 
		cmd = token[0]; 			// this gives us the command letter 
	// else if (s[0]==':') {
		// putchar(':');
		// strcpy(record, s); 			// copy record in case of the L command (load_record) 
	// }
	else 
		cmd = '~';					// custom char for empty cmd line 
	
	token = strtok(NULL, delim);	// parse the 2nd token (NULL means continue from prev pos) 
	if (token!=NULL) {
		strcpy(operand1, token); 	// if it exists, save it as "operand1" 
	}
	
	token = strtok(NULL, delim);	// parse the 3nd token (NULL means continue from prev pos) 
	if (token!=NULL) {
		strcpy(operand2, token); 	// if it exists, save it as "operand2" 
	}
	
	token = strtok(NULL, delim);	// parse the 4th token (NULL means continue from prev pos) 
	if (token!=NULL) {
		strcpy(operand3, token); 	// if it exists, save it as "operand3" 
	}

	putchar('\n'); 
	putchar('\r'); 
	
	switch (cmd) 	{
		case 'a': 
		case 'A': 
			assemble_code(); 
			break; 
		case 'b': 
		case 'B': 
			run_basic(); 		
			break; 
		case 'c': 
		case 'C': 
			compare_memory(); 
			break;
		case 'd': 
		case 'D': 
			disassemble_code(); 
			break;
		case 'f': 
		case 'F': 
			fill_memory(); 
			break;
		case 'g': 
		case 'G': 
			go(); 
			break;
		case 'h': 
		case 'H': 
			hunt(); 
			break;
		case 'j': 
		case 'J': 
			jump_to_sub(); 
			break;
		case 'l': 
		case 'L': 
			load_record(); 
			break;
		case 'm': 
		case 'M': 
			display_memory(); 
			break;
		case 'n': 
		case 'N': 
			numeric_conversion(s); 
			break;
		case 'r': 
		case 'R': 
			display_register(); 
			break;
		case 's': 
		case 'S': 
			self_test(); 
			break;
		case 't': 
		case 'T': 
			transfer_memory(); 
			break;
		case 'u': 
		case 'U': 
			update_datetime(); 
			break;
		case 'w': 
		case 'W': 
			Run_WozMon(); 
			break;
		case 'x': 
		case 'X': 
			exit_to_BIOS(); 
			break;
		case ':': 
			change_memory(); 
			break;
		case ';': 
			change_registers(); 
			break;
		case '?': 
			help_menu(s); 
			break;
		case '~': 			// ignore empty command lines 
			break; 
		default: 
			puts(UNKNOWN_ERR); 
			break; 
	}
}

void assemble_code(void) {
// syntax: A <addr> <mnemonic> [<argument>]
	// puts(ASSEMBLE_MSG); 
}

void run_basic(void) {
// syntax: B 
	// puts(MSBASIC_MSG); 
	// asm 
	// { 
		// jmp $D000; 
	// }
}

void compare_memory(void) {
// syntax: C <start> <end> <ref>
	puts(COMPMEM_MSG); 
}

void disassemble_code(void) {
// syntax: D [<addr1> [<addr2>]]
	puts(DISASSEM_MSG); 
}

void fill_memory(void) {
// syntax: F <start> <end> <fill>
	unsigned int cnt = 0; 
	unsigned int start = 0; 
	unsigned int end = 0; 
	char val = 0; 
	char oper1_saved[5] = '\0'; 
	char oper2_saved[5] = '\0'; 
	char oper3_saved[5] = '\0'; 
	char *Loc = NULL; 
	char msg[80] = ""; 

	strcpy(oper1_saved,operand1); 
	strcpy(oper2_saved,operand2); 
	strcpy(oper3_saved,operand3); 

	start = ConvertASCtoWord(operand1); 
	end = ConvertASCtoWord(operand2); 
	val = ConvertASCtoByte(operand3); 

	for (cnt = start; cnt <= end; cnt++) {
		Loc = (char *)(cnt); 		// pointer 
		*Loc= val; 							// store value
	}

	strcpy(msg, FILLMEM_MSG); 				// "filling memory range 0x"
	strcat(msg, operand1); 					// start addr
	strcat(msg, TO_MSG); 					// "to 0x" 
	strcat(msg, operand2); 					// end_addr
	strcat(msg, WITH_MSG);					// "with value 0x" 
	if (strlen(operand3)==1)
		putchar('0'); 
	strcat(msg, operand3); 					// value
	strcat(msg, DOTS_MSG); 					// " ... done"
	puts(msg); 

	display_memory(); 

	strcpy(operand1, oper1_saved); 
	strcpy(operand2, oper2_saved); 
	strcpy(operand3, oper3_saved); 
}

void go(void) {
// syntax: G [<dp>]
	// asm{jsr $5000;}; 
	// puts(BCK_GO); 
}

void hunt(void) {
// syntax: H <addr1> <addr2> <pattern>
	puts(HUNT_MSG); 
}

void jump_to_sub(void) {
// syntax: J [<dp>]
	puts(JUMP_MSG); 
}

void load_record(void) {
// syntax: L 
	// L was pressed, so we now expect for an Intel HEX record to be entered. 
	// Intel HEX record format: 
	// Pos 0000000000111111111122222222223333333333444444444455555555556
	// Pos 0123456789012345678901234567890123456789012345678901234567890
	//     :1856200020A9208DE07FC220E220A5012908C220F00382B4FFE220A9DB

	// :			start code: ASCII char ':'
	// 18			hex byte count: 1 hex byte indicating # of hex bytes in payload (0x18 = 24 hex bytes = 48 ASCIIchars)
	// 5620			hex start addr: 2 hex bytes indicating the offset from the base address (base addr default =0) 
	// 00			hex record type: 1 hex byte (00..05) indicating the record type (00=data; 01=EOF) 
	// 20A9..20A9	payload: 24 hex bytes of data = 48 ASCII chars 
	// DB			checksum: 18+80+48+..+20+A9=C25; Take LSB (=25) and 2's complement (=0x00-0x25 = DB)
	// char size; 				// hex byte count (0..FF) converted to decimal  
	// unsigned int startaddr; 			// hex start addr (00..FFFF) 
	// char rectype; 				// 00 for data; 01 for EoF; 02..05 are not used
				     //0000000000111111111122222222223333333333444444444455555555556
					 //0123456789012345678901234567890123456789012345678901234567890
	char *IntHexStr = ":1856200020A9208DE07FC220E220A5012908C220F00382B4FFE220A9DB";
	char cnt; 
	char *loc = (char *)(0x3000); 

/* 	
	cmd = getchar(); 					// get char and convert to lower case 
	if (cmd != '\0')					// if char typed 
	{
 		switch (cmd) 
		{
			case '\r':					// <LF>/<CR> indicates "completed" cmd string
			// case '\n':					// so we terminate and parse it 
				INPUT_BUFFER[WR_IDX-1] = '\0'; 	// replace CR/LF with \0
				strcpy(command, INPUT_BUFFER); 
				parse_cmd(command); 	// parse INPUT_BUFFER 
				WR_IDX = 0; 			// back to SoB 
				RD_IDX = 0; 
				ACIA_prompt(); 			// write a cmd prompt on next line 
				break;
			default:					// anything else (e.g. regular chars ...) 
				putchar(cmd); 			// echo the char 
			break;
		}
	}
 */	
/* 	// strncpy(rectype, record+7, 2); 		// start at char 7, length 2 chars 
	// strcpy(rectype+2, 0x00); 			// add a null char after it 
	// unsigned char rectype = ConvertASCtoByte(rectype);	//  convert to 0x00 or 0x01 
	putstring(STORING_MSG); 			// "Storing "
	strncpy(bytecount, record+1, 2); 	// start at char 1, length 2 chars 
	strcpy(bytecount+2, 0x00); 			// add null char after it 
	putchar(record[0]); 
	putchar(record[1]); 
	// size = ConvertASCtoByte(bytecount); // convert to decimal 
	// DisplByteinHex(size); 				// "18" -> 24
	putstring(VALUES_MSG);				// " values " 
	strncpy(payload, record+9, 48); 	// start at char 9, length 48 chars 
	strcpy(payload+48, 0x00); 			// add a null char after it 
	putstring(payload); 				// "20A9208DE07FC220E220A5012908C220F00382B4FFE220A9"
	putstring(ATADDR_MSG);				// " at starting address 0x"
	strncpy(address, record+3, 4); 		// start at char 3, length 4 chars 
	strcpy(address+4, 0x00); 			// add a null char after it 
	putchar(record[3]); 
	putchar(record[4]); 
	putchar(record[5]); 
	putchar(record[6]); 
	// startaddr = ConvertASCtoWord(address); 	// convert '5620' to decimal word 0x5620
	// DisplWordinHex(startaddr); 			// "5620"
 */	
	puts('\0'); 						// next line 
	putstring(STORING_MSG); 			// "Storing "
	// puts('\0'); 						// next line 
	// for (cnt=0; cnt<=0x18; cnt++) {
		// loc = (char *)(0x3000+cnt); 
		// *loc = val; 
	// }
	// if (s[0]>='0' && s[0]<='9') value += (s[0] - 0x30) * 0x10; 
	// if (s[0]>='A' && s[0]<='F') value += (s[0] - 0x37) * 0x10; 
	putstring(IntHexStr); 
	// puts('\0');
		   // *loc = (IntHexStr[ 9]-0x30)*0x10 + (IntHexStr[10]-0x30); 	// 20
	// loc++; *loc = (IntHexStr[11]-0x37)*0x10 + (IntHexStr[12]-0x30); 	// A9
	// loc++; *loc = (IntHexStr[13]-0x30)*0x10 + (IntHexStr[14]-0x30); 	// 20
	// loc++; *loc = (IntHexStr[15]-0x30)*0x10 + (IntHexStr[16]-0x37); 	// 8D
	// loc++; *loc = (IntHexStr[17]-0x37)*0x10 + (IntHexStr[18]-0x30); 	// E0
	// loc++; *loc = (IntHexStr[19]-0x30)*0x10 + (IntHexStr[20]-0x37); 	// 7F
	// loc++; *loc = (IntHexStr[21]-0x37)*0x10 + (IntHexStr[22]-0x30); 	// C2
	// loc++; *loc = (IntHexStr[23]-0x30)*0x10 + (IntHexStr[24]-0x30); 	// 20
	// loc++; *loc = (IntHexStr[25]-0x37)*0x10 + (IntHexStr[26]-0x30); 	// E2
	// loc++; *loc = (IntHexStr[27]-0x30)*0x10 + (IntHexStr[28]-0x30); 	// 20
	// loc++; *loc = (IntHexStr[29]-0x37)*0x10 + (IntHexStr[30]-0x30); 	// A5
	// loc++; *loc = (IntHexStr[31]-0x30)*0x10 + (IntHexStr[32]-0x30); 	// 01
	// loc++; *loc = (IntHexStr[33]-0x30)*0x10 + (IntHexStr[34]-0x30); 	// 29
	// loc++; *loc = (IntHexStr[35]-0x30)*0x10 + (IntHexStr[36]-0x30); 	// 08
	// loc++; *loc = (IntHexStr[37]-0x37)*0x10 + (IntHexStr[38]-0x30); 	// C2
	// loc++; *loc = (IntHexStr[39]-0x30)*0x10 + (IntHexStr[40]-0x30); 	// 20
	// loc++; *loc = (IntHexStr[41]-0x37)*0x10 + (IntHexStr[42]-0x30); 	// F0
	// loc++; *loc = (IntHexStr[43]-0x30)*0x10 + (IntHexStr[44]-0x30); 	// 03
	// loc++; *loc = (IntHexStr[45]-0x30)*0x10 + (IntHexStr[46]-0x30); 	// 82
	// loc++; *loc = (IntHexStr[47]-0x37)*0x10 + (IntHexStr[48]-0x30); 	// B4
	// loc++; *loc = (IntHexStr[49]-0x37)*0x10 + (IntHexStr[50]-0x37); 	// FF
	// loc++; *loc = (IntHexStr[51]-0x37)*0x10 + (IntHexStr[52]-0x30); 	// E2
	// loc++; *loc = (IntHexStr[53]-0x30)*0x10 + (IntHexStr[54]-0x30); 	// 20
	// loc++; *loc = (IntHexStr[55]-0x37)*0x10 + (IntHexStr[56]-0x30); 	// A9
	puts(DONE_MSG); 					// 
}

void display_memory(void) {
	// syntax: M [<addr1> [<addr2>]]
	// the parse function split the cmd line into 3 (global variables) tokens
	// they are: 'cmd' and 'operand1' and 'operand2', all unsigned char * (=strings) 
	// Note that operand1 and operand2 are ASCII strings and not HEX numbers!
	// startAddr and endAddr are their HEX equivalents 
	unsigned int startAddr = 0x0000; 					// operand1 in 2byte hex integer 
	unsigned int endAddr = 0x0000; 						// operand2 in 2byte hex integer 
	char err1 = 0; 							// 0/1 boolean-type flag; 0 = no err; 1 = err 
	char err2 = 0; 							// 0/1 boolean-type flag; 0 = no err; 1 = err 
	unsigned int NrOfLines = 0;							// number of 16 byte-lines of memory content to show 
	unsigned int cnt = 0; 								// generic counter 
	char swap = 0; 							// operand2 < operand1 flag 
	char msg[80]; 								// msg showing the command and mem range 

	if (strlen(operand1)==0) {									// no operand1 provided
		puts(DISPMEMSYNTAX_MSG); 						// write command syntax
	}		
	else { 
		err1 = check_address(operand1); 				// is operand1 a valid address? 

		if (strlen(operand2)==0) {								// if no operand2 provided
			strcpy(operand2, operand1); 				// duplicate operand1 
		}
		else {
			err2 = check_address(operand2); 			// operand2 provided, check if valid address
		}

		if (err1 || err2) {								// if either operand is invalid 
			puts(INVADDR_ERR); 							// display a "syntax error" msg 
		}
		else {											// if both valid 
			startAddr = ConvertASCtoWord(operand1); 	// convert to 2byte integer 
			endAddr = ConvertASCtoWord(operand2); 		// convert to 2byte integer

			if (startAddr > endAddr) {					// swap if startAddr > endAddr 
				unsigned int temp = 0; 
				temp = startAddr; 
				startAddr = endAddr; 
				endAddr = temp; 
				swap = 1; 
			}

			startAddr = startAddr & 0xFFF0; 			// always have startAddr start at 0x0
			endAddr = endAddr | 0x000F; 				// always hav endAddr end at 0xF 

			operand1[0] = toupper(operand1[0]); 		// convert to upper case 
			operand1[1] = toupper(operand1[1]); 
			operand1[2] = toupper(operand1[2]); 
			operand1[3] = '0'; 							// ensure starting with 0
			operand2[0] = toupper(operand2[0]); 
			operand2[1] = toupper(operand2[1]); 
			operand2[2] = toupper(operand2[2]); 
			operand2[3] = 'F';							// ensure ending with F
			
			puts('\0'); 
			strcpy(msg, DISPMEM_MSG); 					// Building the "display mem range" msg, len = 23 
			// strcat(msg, FROM); 						// adding " from 0x", len = 8
			if (swap==0) 
				strcat(msg, operand1); 
			else 
				strcat(msg, operand2); 						
														// adding smallest operand, len = 4
			strcat(msg, TO_MSG);						// adding " to 0x", len = 6
			if (swap==0) 
				strcat(msg, operand2); 
			else
				strcat(msg, operand1); 
														// adding largest operand, len = 4
			puts(msg); 									// output entire msg, total len = 45 

			NrOfLines = (endAddr - startAddr) / 16;		// how many lines of 16 bytes are required? 
			for (cnt=0 ; cnt<=NrOfLines; cnt++) {		// for every line of 16 bytes 
				if ((cnt % 16) == 0) {					// put a hdr line for every 16*16 bytes 
					putchar('\r'); 
					putchar('\n'); 
					puts(MEMHDR);							
					puts(MEMSEP); 
				}
				DisplayCurrAddr(startAddr+cnt*16);		// display mem range + content in HEX and ASCII
			}
		}
	}
	operand1[0]='\0'; 		// clear the 3 operands 
	operand2[0]='\0'; 		// clear the 3 operands 
	operand3[0]='\0'; 		// clear the 3 operands 
}

void numeric_conversion(char *s) {
// syntax: N <radix><value> with <radix>=[0x=HEX]/[-=DEC]/[0O=OCT]/[0b=BIN]
	puts(NUMCONV_MSG); 
	if (s[2] == '0') 
	{
		// if (s[3]=='x') 
			// ConvertHex(s); 
		// if (s[3]=='O') 
			// ConvertOct(s); 
		// if (s[3]=='b') 
			// ConvertBin(s);
	}
	else 
		puts(SYNTAX_ERR); 
}

void display_register(void) {
	// entering r/R command from the command prompt 
	// Example: 
	//	"  PB  PC   NVmxDIZC  .C   .X   .Y   SP   DP  DB"
	//	"  00 83FA  00110101 00CA 0000 0ABC 7FE0 002D 00
	puts(DISPREG_MSG); 			// starting the msg, len = 23 
	putchar('\r');
	putchar('\n');
	puts(REGHDR); 				// adding "  PB  PC   NVmxDIZC  .C   .X   .Y   SP   DP  DB"
	putchar(' ');
	putchar(' ');
	// display program bank register 
		// asm {
			// php; 
			// sep #$20; 		// 8 bit A
			// phk;			// push PB register onto stack 
			// pla;			// pull ToS into C register 
			// sta $1200;		// store C register into shadow PB register 
			// plp;
		// }
		DisplByteinHex(*(PB_REGISTER)); 
		putchar(' ');
	// display program counter 
		// asm  {
			// php;
			// rep #$20;		// 16 bit C register 
			// per #0;			// push program counter + 0 onto stack 
			// pla;			// pull ToS into C register 
			// sta $1201;		// store C register inot shadow PC register 
			// plp;
		// }
		DisplWordinHex(*(PC_REGISTER)); 
		putchar(' ');
		putchar(' ');
	// display status register 
		// asm {
			// php; 			// push SR onto stack to save CPU status 
			// php				// push SR onto stack (8 bits)
			// sep #$20		// 8 bit A register (changes SR bits)
			// pla; 			// pull ToS into A register
			// sta $1203; 		// store C register into shadow SR 
			// plp				// restore system status 
		// } 
		DisplByteinBin(*(STAT_REGISTER)); 
		putchar(' ');
	// display C register 
		// asm {
			// php;
			// rep #$20;		// 16 bit C register 
			// sta $1204;		// store C regiser into shadow C register 
			// plp
		// }
		DisplWordinHex(*(C_REGISTER)); 
		putchar(' ');
	// display X register 
		// asm { 
			// php;
			// rep #$10;		// 16 bit X/Y register 
			// stx $1206;		// store X regiser into shadow X register 
			// plp;
		// } 
		DisplWordinHex(*(X_REGISTER)); 
		putchar(' ');
	// display Y register 
		// asm { 
			// php;
			// rep #$10;		// 16 bit X/Y register 
			// sty $1208;		// store Y register into shadow Y register 
			// plp;
		// } 
		DisplWordinHex(*(Y_REGISTER)); 
		putchar(' ');
	// display stack pointer 
		// asm {  
			// php;
			// rep #$20;		// 16 bit C register 
			// tsc; 			// transfer Stack Ptr to C register 
			// sta $120A;		// store C register in shadow stack pointer register 
			// plp;
		// }
		DisplWordinHex(*(STACK_PTR)); 
		putchar(' ');
	// display direct page register 
		// asm {
			// php;
			// rep #$20;		// 16 bit C register 
			// tdc;			// pull direct page register into C register 
			// sta $120C;		// store C register into shadow direct page register 
			// plp;
		// }
		DisplWordinHex(*(DP_REGISTER)); 
		putchar(' ');
	// display data bank register
		// asm {
			// php;
			// sep #$20;		// 8 bit A regsiter 
			// phb;			// push data bank register onto stack 
			// pla;			// pull ToS into A register 
			// sta $120E;		// store A register into shadow data bank register 	
			// plp;
		// }
		DisplByteinHex(*(DB_REGISTER)); 
		putchar(' '); 
}

void self_test(void) {
// syntax: S
	// check for RAM 
	// check for VIA 
	puts(SELFTEST_MSG);
}

void transfer_memory(void) {
// syntax: T <start> <end> <target>
	puts(TRANSMEM_MSG); 
}

void update_datetime(void) {
// syntax: U 
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

void Run_WozMon(void) {
	asm {jsr $FA00;};
}

void exit_to_BIOS(void) {
// syntax: X
	puts(EXIT_MSG); 
}

void change_memory(void	) {
// syntax: : [<addr> <operand> [<operand>]...]
	// 0000000000111111111122222222223333333333444444444455555555556
	// 0123456789012345678901234567890123456789012345678901234567890
	// :1880480020A9208DE07FC220E220A5012908C220F00382B4FFE220A9DB
	
	// :			start code: ASCII char ':'
	// 18			hex byte count: 1 hex byte indicating # of hex bytes in payload (0x18 = 24 hex bytes = 48 ASCII chars)
	// 8048			hex address: 2 hex bytes indicating the offset from the base address (base addr default =0) 
	// 00			hex record type: 1 hex byte (00..05) indicating the record type (00=data; 01=EOF) 
	// 20A9..20A9	payload: 24 hex bytes of data = 48 ASCII chars 
	// DB			checksum: 18+80+48+..+20+A9=C25; Then take LSB (=25). Then 2's complement (DB)

	unsigned int Addr = 0; 
	char Value = 0; 
	char *Loc = NULL; 
	// unsigned char msg[80]; 
	// unsigned char err1 = 0; 					// 0/1 boolean-type flag; 0 = no err; 1 = err 
	// unsigned char err2 = 0; 					// 0/1 boolean-type flag; 0 = no err; 1 = err 
	
	// operand1 and operand2 are global variables given values by the 
	// token splitting of the command line 
	// if (strlen(operand1)==0) {						// no operand1 provided
		// puts(CHNGMEMSYNTAX_MSG); 					// show command syntax 
	// }
	// else {
		// err1 = check_address(operand1); 			// operand1 provided. Valid address? 

		// if (strlen(operand2)==0) {					// no operand2 provided
			// puts(CHNGMEMSYNTAX_MSG); 				// show command syntax 
		// }
		// else {
			// err2 = check_value(operand2); 			// operand2 provided. Valid value? 

			// if (err1 || err2) {						// if an operand was invalid 
				// err1 ? puts(INVADDR_ERR) : puts(INVVAL_ERR) ; 	// display a "syntax error" msg 
			// }
			// else {									// both operands are valid 
				// Addr = ConvertASCtoWord(operand1); 	// convert to 2byte integer 
				// Value = ConvertASCtoByte(operand2);	// convert to 1 byte integer
				// Loc = (unsigned char *)(Addr); 
				// *Loc= Value; 

				// puts('\0');
				// strcpy(msg, STORED_MSG);
				// strcat(msg, operand2); 
				// strcat(msg, LOCATION_MSG);
				// strcat(msg, operand1);
				// strcat(msg, DOTS_MSG); 
				// puts(msg); 

				// strcpy(operand2, operand1); 		// 
				// display_memory();
			// }
		// }
	// }
	Addr = ConvertASCtoWord(operand1); 	// convert to 2 byte integer 
	Value = ConvertASCtoByte(operand2);	// convert to 1 byte integer
	Loc = (char *)(Addr); 
	*Loc= Value; 
	// operand1[0]='\0'; 		// clear the 3 operands 
	// operand2[0]='\0'; 		// clear the 3 operands 
	// operand3[0]='\0'; 		// clear the 3 operands 
}

void change_registers(void) {
// syntax: ; [PB [PC [.S [.C [.X [.Y [SP [DP [DB]]]]]]]]]
	puts(CHNGREG_MSG); 
}

void help_menu(char *s) {
// syntax: ?
	if (s[1] != '\0') 
		puts(SYNTAX_ERR); 
	else 
		puts(HELP_MENU); 
}

char check_address(char *addr) {
	// function to check if addr is a string of the format "HHHH" 
	// with each H a valid HEX digit. 
	// Will also pad with leading zeros where needed. 
	char syntax_error = 0; 					// assume no err

	// note that addr is a string here, not yet converted to a hex value
	if (strlen(addr)>4) {
		puts(ILLADD_ERR1); 
	}
	
	if (strlen(addr)==3) {								// e.g. 123
			addr[3]=addr[2];							// 123 -> 1233
			addr[2]=addr[1];							// 1233 -> 1223
			addr[1]=addr[0];							// 1223 -> 1123
			addr[0]='0';								// 1123 -> 0123
	}
	else if (strlen(addr)==2) {							// e.g. 12
			addr[3]=addr[1];							// 12 -> 12_2
			addr[2]=addr[0];							// 12_2 -> 1212
			addr[1]='0';								// 1212 -> 1012
			addr[0]='0';								// 1012 -> 0012
	}
	else if (strlen(addr)==1) {							// e.g. 1
			addr[3]=addr[0];							// 1 -> 1__1
			addr[2]='0';								// 1__1 -> 1_01
			addr[1]='0';								// 1_01 -> 1001
			addr[0]='0';								// 1001 -> 0001 
	}
	addr[4]='\0';										// we have 4 chars, add a terminator 
	
	if (!(my_isxdigit(addr[0])) || 						// are all addr digits valid HEX?
		!(my_isxdigit(addr[1])) || 
		!(my_isxdigit(addr[2])) || 
		!(my_isxdigit(addr[3])) ) {
		puts(ILLADD_ERR2); 								// output an err msg 
		syntax_error = 1; 								// set err flag 
	}
	
	return syntax_error; 
}

char check_value(char *val) {
	// function to check if val is a string with two valid HEX digits. 
	// Will also pad with leading zero when needed. 
	char syntax_error = 0; 			// assume no err

	// note that val is a string here, not yet converted to a hex value
	if (strlen(val)>2) {
		puts(ILLADD_ERR1); 
	}
	
	if (strlen(val)==1) {						// e.g. 1
		val[1]=val[0];							// 1 -> 11
		val[0]='0';								// 11 -> 01
	}
	val[2]='\0';								// we have 2 chars, add a terminator 
	
	if (!(my_isxdigit(val[0])) || 				// are both digits valid HEX?
		!(my_isxdigit(val[1]))) {
		puts(ILLADD_ERR2); 						// if not, output an err msg 
		syntax_error = 1; 						// and set err flag 
	}
	
	return syntax_error; 
}

void check_mnemonic(char *s) {
}

void check_pattern(char *s) {
}

void check_operand(char *s) {
}

void DisplByteinHex(char value) {
	// tested - working OK
	// outputs a HEX byte value (0..255 or 0x00..0xFF) in ASCII 
	// values 0..9 become 0x30..0x39 by adding 0x30
	// values A..F/a..f become 0x41..0x46 by adding 0x37
	char digit; 
	digit = value >> 4; 											// high nibble 
	(digit > 0x09) ? putchar(digit + 0x37) : putchar(digit + 0x30); // convert/output in ASCII 
	digit = value & 0x0F; 											// low nibble
	(digit > 0x09) ? putchar(digit + 0x37) : putchar(digit + 0x30); // convert/output in ASCII
}

void DisplWordinHex(unsigned int value) {
	// tested - working OK
	// outputs a HEX word value (0..65535 or 0x0000..0xFFFF) in ASCII
	// values 0..9 become 0x30..0x39 by adding 0x30
	// a..f/A..F become 0x41..0x46 by adding 0x37
	char digit; 
	digit = value >> 12; 											// hi byte, hi nibble
	(digit > 0x09) ? putchar(digit + 0x37) : putchar(digit + 0x30); // convert/output in ASCII 
	digit = (value & 0x0F00) >> 8; 									// hi byte, lo nibble
	(digit > 0x09) ? putchar(digit + 0x37) : putchar(digit + 0x30); // convert/output in ASCII
	digit = (value & 0x00F0) >> 4; 									// lo byte, hi nibble
	(digit > 0x09) ? putchar(digit + 0x37) : putchar(digit + 0x30); // convert/output in ASCII 
	digit = (value & 0x000F); 										// lo byte, lo nibble 
	(digit > 0x09) ? putchar(digit + 0x37) : putchar(digit + 0x30); // convert/output in ASCII
}

void DisplByteinBin(char value) {
	//
	char bit = 0; 
	char mask = 0x80;
	for (bit=0; bit<=7; bit++) {
		!(value & mask) ? putchar('0') : putchar('1'); 
		mask = mask >> 1;
	}
}

void DisplWordinBin(unsigned int value) {
	//
	char bit = 0; 
	unsigned int mask = 0x8000;
	for (bit=0; bit<=15; bit++) {
		!(value & mask) ? putchar('0') : putchar('1'); 
		mask = mask >> 1;
	}
}

char ConvertASCtoByte(char *s) {
	// converts 2 HEX digit string (no leading 0x) (00..FF) to byte value (0..255) 
	char value = 0; 

	if (strlen(s)==1) {						// e.g. '5_' (_ = \0)
		s[1]=s[0]; 							// '5_' -> '55'
		s[0]='0';							// '55' -> '05'
		s[2]='\0';							// '05' -> '05_'
	}

	// '0'..'9' = 0x30..0x39 
	// 'a'..'f' = 0x41..0x46 = 0x37+10..16
	// 'A'..'F' = 0x61..0x66 = 0x57+10..16
	if (s[0]>='0' && s[0]<='9') value += (s[0] - 0x30) * 0x10; 
	if (s[0]>='A' && s[0]<='F') value += (s[0] - 0x37) * 0x10; 
	if (s[0]>='a' && s[0]<='f') value += (s[0] - 0x57) * 0x10; 

	if (s[1]>='0' && s[1]<='9')	value += (s[1] - 0x30) * 0x01; 
	if (s[1]>='A' && s[1]<='F') value += (s[1] - 0x37) * 0x01; 
	if (s[1]>='a' && s[1]<='f') value += (s[1] - 0x57) * 0x01; 

	return value; 
}

unsigned int ConvertASCtoWord(char *s) {
	// converts 4 HEX digit string (0000..FFFF) to a word value (0..65535) 
	unsigned int value = 0; 
	
	// '0'..'9' = 0x30..0x39 
	// 'a'..'f' = 0x41..0x46 = 0x37+10..16
	// 'A'..'F' = 0x61..0x66 = 0x57+10..16
	if (s[0]>='0' && s[0]<='9') value += (s[0] - 0x30) * 0x1000; 
	if (s[0]>='A' && s[0]<='F') value += (s[0] - 0x37) * 0x1000; 
	if (s[0]>='a' && s[0]<='f') value += (s[0] - 0x57) * 0x1000; 

	if (s[1]>='0' && s[1]<='9')	value += (s[1] - 0x30) * 0x0100; 
	if (s[1]>='A' && s[1]<='F') value += (s[1] - 0x37) * 0x0100; 
	if (s[1]>='a' && s[1]<='f') value += (s[1] - 0x57) * 0x0100; 
	
	if (s[2]>='0' && s[2]<='9') value += (s[2] - 0x30) * 0x0010; 
	if (s[2]>='A' && s[2]<='F') value += (s[2] - 0x37) * 0x0010; 
	if (s[2]>='a' && s[2]<='f') value += (s[2] - 0x57) * 0x0010; 

	if (s[3]>='0' && s[3]<='9') value += (s[3] - 0x30) * 0x0001; 
	if (s[3]>='A' && s[3]<='F') value += (s[3] - 0x37) * 0x0001; 
	if (s[3]>='a' && s[3]<='f') value += (s[3] - 0x57) * 0x0001; 

	return value;
}

void DisplayCurrAddr(unsigned int startAddr) {
	// one line of memory content ... format: 
	//  00000000001111111111222222222233333333334444444444555555555566666666667777777777
	//  01234567890123456789012345678901234567890123456789012345678901234567890123456789
	// "00:7000 : 57 44 43 36 35 43 38 31 - 36 2D 62 61 73 65 64 00 : WDC65C816-based "
	unsigned int cnt = 0; 		// generic counter 
	char cont = 0; 	// content / value in a byte in memmory 
	char digit = 0; 	// one of the 4 digits of an address 
	putchar('0'); 				// two chars for bank address (0x00 .. 0xFF) 
	putchar('0'); 
	putchar(':'); 				// bank addr/address separator 
	DisplWordinHex(startAddr);	// mem range without leading 0x
	putchar(' '); 				// separator to group of 16 HEX bytes  
	putchar('|'); 
	putchar(' '); 
	for (cnt = 0x00; cnt <= 0x0F ; cnt++) 				// 16 HEX bytes 
	{
		cont = *(char *)(startAddr + cnt); 	// get byte content 
		DisplByteinHex(cont); 							// display it in ASCII/HEX
		putchar(' '); 									// interbyte spacer
		if (cnt == 0x07) 								// separator after 8 bytes 
		{
			putchar('-'); 								
			putchar(' '); 
		}
	}
	putchar('|'); 										// separator to 16 ASCII values 
	putchar(' '); 
	for (cnt=0x00 ; cnt<=0x0F ; cnt++)					// 16 memory values as ASCII 
	{
		cont = *(char *)(startAddr + cnt); 	// byte content 
		if ( (cont>=0x20) && (cont<=0x7E) )				// print if printable ASCII 
			putchar(cont);								
		else											// else print '.' 
			putchar('.') ; 								
	}
	putchar('\r'); 
	putchar('\n'); 
}

char my_isxdigit(char c) {
	return (((c >= '0') && (c <= '9')) || 
			((c >= 'a') && (c <= 'f')) ||
			((c >= 'A') && (c <= 'F')));	
}