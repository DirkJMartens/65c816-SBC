/** @file CMDMON.h
*
* @brief Header file for the command interpreter 
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Dirk Martens. No right reserved. Use at your own risk. 
*/

#ifndef CMDMON_H
#define CMDMON_H 

#define PB_REGISTER		(		  (char *)0x1200)		// shadow PB register 	(1 byte) 
#define PC_REGISTER 	((unsigned int  *)0x1201)		// shadow PC register 	(2 bytes) 
#define STAT_REGISTER 	(         (char *)0x1203)		// shadow PC register 	(1 byte)
#define C_REGISTER 		((unsigned int  *)0x1204)		// shadow C register 	(2 bytes)  
#define X_REGISTER 		((unsigned int  *)0x1206)		// shadow X register 	(2 bytes)
#define Y_REGISTER 		((unsigned int  *)0x1208)		// shadow Y register 	(2 bytes)
#define STACK_PTR 		((unsigned int  *)0x120A)		// shadow Stack pointer (2 bytes)
#define DP_REGISTER 	((unsigned int  *)0x120C)		// shadow DP register 	(2 bytes)
#define DB_REGISTER 	(         (char *)0x120E)		// shadow DB register 	(1 byte)

// defined in C_template_816.c
extern char operand1[]; 		// cmd line param/operand/addr 
extern char operand2[]; 
extern char operand3[]; 
// char *IntHexStr = ":1856200020A9208DE07FC220E220A5012908C220F00382B4FFE220A9DB";

// forward declarations and prototypes
void parse_cmd(char *);					// parses the cmd line string and jumps to subr
void assemble_code(void); 				// convert ASM mnemonics into assembly code (a/A)
void run_basic(void);					// run MSBasic (b/B) 
void compare_memory(void); 				// compare two sections of memory (c/C)
void disassemble_code(void); 			// convert assembly code to ASM mnemonics (d/D) 
void fill_memory(void);					// fill a memory range with a number (f/F) 
void go(void);							// start executing code from an address (g/G)  
void hunt(void); 						// search memory for a pattern (h/H) 
void load_record(void); 				// load mem addr <addr> with <value> 
void jump_to_sub(void); 				// jump to subroutine and start executing (j/J) 
void display_memory(void); 				// display the content of a memory range (m/M) 
void numeric_conversion(char *); 		// convert numbers between HEX/OCT/BIN/DEC (n/N) 
void display_register(void); 			// display the content of the CPU registers (r/R) 
void selftest_diags(void); 				// 
void self_test(void); 					// 
void transfer_memory(void); 			// copy one memory range to another (t/T)
void update_datetime(void); 			// update the system date/time 
void Run_WozMon(void); 					// run WozMon
void exit_to_BIOS(void); 				// exit to BIOS (x/X) 
void change_memory(void);				// edit the content of memory addresses (>)
void change_registers(void); 			// change content of one/any of the register (' ')
void help_menu(char *);					// display menu of valid commands and their syntax 
char check_address(char *);				// 
void check_mnemonic(char *);			// 
void check_pattern(char *);				// 
void check_operand(char *);				// 
char check_value(char *);				//  
void DisplByteinHex(char);				// 
void DisplWordinHex(unsigned int);		// 
void DisplByteinBin(char); 				// 
void DisplWordinBin(unsigned int);		// 
char ConvertASCtoByte(char *);			// 
unsigned int ConvertASCtoWord(char *); 	// 
void DisplayCurrAddr(unsigned int); 	// 
char my_isxdigit(char);					// 

#endif		// end CMDMON_H
