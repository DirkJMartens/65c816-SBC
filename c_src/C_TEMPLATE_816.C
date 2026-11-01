// checking compilation / linking command line options 
//** make active for WDC C compiler, comment out for CC65 compiler 
// #ifndef USING_816
// EXIT         "Not Valid Processor: Use -DUSING_816, etc. !"
// #endif

// include files 
// #include <stdio.h>			// do not uncomment as putchar/getchar/... were rewritten 
// #include <ctype.h> 			// tolower() function 
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\mystrings.h"			// strings for welcome screen, LCD, console menu ... 
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_src\lcd.c"				// to use the 2x16 LCD display connected to PORTA 
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_src\ACIA.c"				// to use the ACIA as serial port for the console 
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_src\cmdmon.c"				// code to handle console commands 
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_src\RTC.c"				// code to handle ICM7170 real time clock 
#include <string.h> 

#define	CLK_RATE 1000000		// CPU clock speed, needed for the msdelay() function 

// defining heap start and end locations
// DO NOT SPLIT ACROSS 2 LINES 
// void *heap_start = ( void * )0xa000, *heap_end = ( void * )0xd000;

// forward declarations and prototypes 
void msdelay(unsigned short);
void setup(void); 
void loop(void); 
interrupt void IRQPinHandler(void);	// triggered when ACIA RX BUFFER is full 
void RTC_Plus_1Sec(void); 
void RTC_Minus_1Sec(void); 
void RTC_Plus_1Min(void); 
void RTC_Minus_1Min(void); 
void RTC_Plus_1Hour(void); 
void RTC_Minus_1Hour(void); 

// START is the code executed after a hardware reset of the CPU: 
// 		It disables interrupts, switches CPU to native mode, clears decimal mode, 
// 		initialized stack, and makes A/X/Y registers 16 bits. 
// Lastly, it (short or long) jumps to the main() function in this program. 
// Defined in the WDC_CSTARTUP816.ASM program and included by the linker 
extern void START (void);

// char to store the command on the cmd line, situated in DATA section of RAM 
char cmd = 0; 				// variable to process circular buffer one by one 

// 3 strings to store cmd line parameters, situated in DATA section of RAM 
char operand1[5]= "";		// parameter / operand / address specified on the cmd line 
char operand2[5]= ""; 
char operand3[5]= ""; 

char command[256] = ""; 

// CODE section - ORG 0x8000
// ****************************************************************************
//                       M A I N   P R O G R A M
// ****************************************************************************
int main (void)
{
	setup(); 			// do "setup" once 

	for (;;)  
		loop();			// do "loop" forever 

	return 0; 
}  // End of Main() 

// ****************************************************************************
//                       S E T U P      P R O C E D U R E 
// ****************************************************************************
void setup (void)
{
	char strng[15]=""; 
	asm {sei; }; 				// disable interrupts ("set interrupt disable") 
	// setup VIA2 
						// VIA2 drives the 2x16 LCD (on PORTB) 
						// VIA2 drives the 8 LED bar graph (on PORTA) 
	VIA2[DDRB] = 0xFF; 			// PORTB all outputs (1=out; 0=in) 
	VIA2[DDRA] = 0xFF;			// PORTA all outputs 
	VIA2[PORTB] = 0x00; 		// PORTB all off 
	VIA2[PORTA] = 0x00; 		// PORTA all off  
	VIA2[IER] = 0x00; 			// disable all VIA2 interrupts 
						// init LCD - LCD can be used for debugging or as display 
	lcd_init(); 				// 4bits/2x16chars/5x8font/cursor+blink on/cls/cursor home 
	lcd_boot_msg(); 			// write a 2-line msg on lcd 
	lcd_set_cur_pos(0,0); 		// cursor to home position 
						// init VIA Shift Register to use with CA1/CA2 (VIA pins 40/39)
	VIA2[PCR] &= CA2_HIGH_OUT; 	// make pin 39 high 
						// init ACIA - ACIA is used a serial port for a console window 
	ACIA_init(); 				// reset ACIA/8N1/115.2K/no echo/no interrupt
								// also init circular buffer RD_IDX to 0 
	ACIA_bootscreen(); 			// output boot screen welcome msg on console 
	ACIA_prompt(); 				// output command prompt 
						// init the RTC using the compilation date/time as initial values
	// RTC_Set_Time(); 
	// RTC[RTC_ISMR]    = 0x00; 					// reset all RTC interrupts 
	// RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	// RTC[RTC_ISMR]    = RTC_ISMR_SEC100; 			// interrupt every 10th of a sec 

	// init the circular input buffer indexes
	RD_IDX = 0; 
	WR_IDX = 0; 
	asm {cli; }; 				// enable interrupts ("CLear Interrupt disable") 
} // end of function setup()

// ****************************************************************************
//                       L O O P      P R O C E D U R E 
// ****************************************************************************
void loop(void)
{
	cmd = getchar(); 					// get char and convert to lower case 
	if (cmd != '\0')					// if char typed 
	{
 		switch (cmd) 
		{
			case '+': 					// including here so no Enter is needed 
				RTC_Plus_1Sec(); 		// increase RTC time by 1 sec
				putchar(cmd); 			// echo the char 
				break; 
			case '-': 
				RTC_Minus_1Sec(); 		// decrease RTC time by 1 sec 
				putchar(cmd); 			// echo the char 
				break; 
			case ']': 					
				RTC_Plus_1Min(); 		// increase RTC time by 1 min
				putchar(cmd); 			// echo the char 
				break; 
			case '[': 
				RTC_Minus_1Min(); 		// decrease RTC time by 1 min
				putchar(cmd); 			// echo the char 
				break; 
			case '}': 					
				RTC_Plus_1Hour(); 		// increase RTC time by 1 hour
				putchar(cmd); 			// echo the char 
				break; 
			case '{': 
				RTC_Minus_1Hour(); 		// decrease RTC time by 1 hour
				putchar(cmd); 			// echo the char 
				break; 
										// ESC = clear input buffer 
			case '\x1B':				// syntax: press ESC
				putchar('^'); 			// caret = ESC 
				putchar('\r'); 
				putchar('\n'); 
				ACIA_prompt(); 			// write a cmd prompt on next line 
				RD_IDX = 0; 			// sets both indexes back to SoB 
				WR_IDX = 0; 			// so next char will be SoB again 
				break; 
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
} // end of function loop()


// ***** Function(s) to handle interrupts if/when enabled and used ***** 
// "interrupt" functions first will automatically first save all registers, then 
// execute the function code below, then restore all registers and return using 
// an "rti" instruction. 
interrupt void IRQPinHandler (void)
{
	// handling ACIA interrupts (one for every char typed) 
	if (ACIA[STAT] & ACIA_RX_FULL)			// ACIA_RX_FULL interrupt is bit 3 
	{
		char x; 
		x = ACIA[STAT]; 					// reading Status Register clears the IRQ 
		INPUT_BUFFER[WR_IDX] = ACIA[DATA];	// store ACIA char in buffer w/ WR_IDX offset 
		WR_IDX++; 							// increment offset, wraps around from $FF to $00 
	} 
	// handling RTC interrupts (typ. 1 per sec) 
	// if (RTC[RTC_ISMR] & RTC_ISMR_GINT) 		// RTC ISMR has global interrupt flag set 
	// {
		// char temp = 0;
		// temp = RTC[RTC_ISMR]; 				// read the ISMR to clear any interrupt 
		// RTC_Time_On_LCD();					// output time/date on LCD display 
	// }
}  // End of Function IRQHandler() 

// ***** Function to wait a specified number of milliseconds ***** 
// "CLK_RATE" value defined in top of this file MUST be adjusted to match system CPU speed 
void msdelay (unsigned short msec)
{
	unsigned long ticks = 0;
	unsigned short wraps = 0;
	unsigned short rem = 0;
	ticks = (CLK_RATE/1000)*(unsigned long)msec;	// number of ticks needed
	wraps = ticks / 0x10000;						// number of timer wrap arounds before done
	rem = ticks % 0x10000;							// spend extra cycles before wrapping
	VIA2[T2CLO] = rem;
	VIA2[T2CHI] = rem >> 8;
	// now, we loop waiting for the timer to expire and each time it does,
	// we decrement 'wraps' till all done.
	do {
		if ( ( VIA2[IFR] & 0x20 ) != 0 ) {
			wraps--;
			VIA2[T2CHI] = 0xff;
		}
	} while( wraps != 0xffff );
}  // End of Function msdelay() 

void RTC_Plus_1Sec(void) {
	char hrs; char mins; char secs; char dow; char dom; char mon; char yr;
	char temp; 
	temp = RTC[RTC_SEC100]; 
	RTC[RTC_CMD_REG] = RTC_1M | RTC_24HR; 	// 0x04+0x01 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	mins = RTC[RTC_MINS];        			// 0..59
	secs = RTC[RTC_SECS];        			// 0..59
	secs++; 
	if (60==secs) {
		secs = 0; 
		mins++;
	}		
	RTC[RTC_SECS] = secs;   				// 0..59
	RTC[RTC_MINS] = mins;	   				// 0..59
	mon  = RTC[RTC_MTHS];         			// 1..12
	dom  = RTC[RTC_DoMS];         			// 1..31
	yr   = RTC[RTC_YEARS];         			// 0..99
	dow  = RTC[RTC_DoW];         			// 0..6	
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

void RTC_Plus_1Min(void) {
	char hrs; char mins; char secs; char dow; char dom; char mon; char yr;
	char temp; 
	temp = RTC[RTC_SEC100]; 
	RTC[RTC_CMD_REG] = RTC_1M | RTC_24HR; 	// 0x04+0x01 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	mins = RTC[RTC_MINS];        			// 0..59
	secs = RTC[RTC_SECS];        			// 0..59
	mins++; 
	RTC[RTC_MINS] = mins;	   				// 0..59
	mon  = RTC[RTC_MTHS];         			// 1..12
	dom  = RTC[RTC_DoMS];         			// 1..31
	yr   = RTC[RTC_YEARS];         			// 0..99
	dow  = RTC[RTC_DoW];         			// 0..6	
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

void RTC_Plus_1Hour(void) {
	char hrs; char mins; char secs; char dow; char dom; char mon; char yr;
	char temp; 
	temp = RTC[RTC_SEC100]; 
	RTC[RTC_CMD_REG] = RTC_1M | RTC_24HR; 	// 0x04+0x01 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	mins = RTC[RTC_MINS];        			// 0..59
	secs = RTC[RTC_SECS];        			// 0..59
	hrs++; 
	RTC[RTC_HRS] = hrs;   					// 0..23
	mon  = RTC[RTC_MTHS];         			// 1..12
	dom  = RTC[RTC_DoMS];         			// 1..31
	yr   = RTC[RTC_YEARS];         			// 0..99
	dow  = RTC[RTC_DoW];         			// 0..6	
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

void RTC_Minus_1Sec(void) {
	char hrs; char mins; char secs; char dow; char dom; char mon; char yr;
	char temp; 
	temp = RTC[RTC_SEC100]; 
	RTC[RTC_CMD_REG] = RTC_1M | RTC_24HR; 	// 0x04+0x01 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	mins = RTC[RTC_MINS];        			// 0..59
	secs = RTC[RTC_SECS];        			// 0..59
	secs--; 
	if (0==secs) {
		secs = 59; 
		mins--;
	}
	RTC[RTC_SECS] = secs;   				// 0..59
	RTC[RTC_MINS] = mins;	   				// 0..59
	mon  = RTC[RTC_MTHS];         			// 1..12
	dom  = RTC[RTC_DoMS];         			// 1..31
	yr   = RTC[RTC_YEARS];         			// 0..99
	dow  = RTC[RTC_DoW];         			// 0..6	
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

void RTC_Minus_1Min(void) {
	char hrs; char mins; char secs; char dow; char dom; char mon; char yr;
	char temp; 
	temp = RTC[RTC_SEC100]; 
	RTC[RTC_CMD_REG] = RTC_1M | RTC_24HR; 	// 0x04+0x01 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	mins = RTC[RTC_MINS];        			// 0..59
	secs = RTC[RTC_SECS];        			// 0..59
	mins--; 
	RTC[RTC_MINS] = mins;	   				// 0..59
	mon  = RTC[RTC_MTHS];         			// 1..12
	dom  = RTC[RTC_DoMS];         			// 1..31
	yr   = RTC[RTC_YEARS];         			// 0..99
	dow  = RTC[RTC_DoW];         			// 0..6	
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

void RTC_Minus_1Hour(void) {
	char hrs; char mins; char secs; char dow; char dom; char mon; char yr;
	char temp; 
	temp = RTC[RTC_SEC100]; 
	RTC[RTC_CMD_REG] = RTC_1M | RTC_24HR; 	// 0x04+0x01 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	mins = RTC[RTC_MINS];        			// 0..59
	secs = RTC[RTC_SECS];        			// 0..59
	hrs--; 
	RTC[RTC_HRS] = hrs;	   					// 0..23
	mon  = RTC[RTC_MTHS];         			// 1..12
	dom  = RTC[RTC_DoMS];         			// 1..31
	yr   = RTC[RTC_YEARS];         			// 0..99
	dow  = RTC[RTC_DoW];         			// 0..6	
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC_Time_On_LCD(); 
	RTC_Time_On_Serial(); 
}

// ***** Function to validate the code does not bump past heap *****
// implemented using inline asm code, hence e.g. the ';' style comments 
// void check_endof_memory (void)
// {
// #asm
// This_project_end:
// WDCMON_RAM_START EQU $7C00
// ;ROMSPACE EQU WDCMON_RAM_START-This_project_end  ;gives space left in the ROM BEFORE WDCMON TABLES
// ;	IF ROMSPACE<0
// ;		EXIT         "Not Enough Memory for This Application - bumping into WDCMON! ! ! ! ! ! ! ! ! ! ! !"
// ;	ENDIF
// #endasm
// }  // End of Function check_endof_memory() 

// end of file C_TEMPLATE_816.C 