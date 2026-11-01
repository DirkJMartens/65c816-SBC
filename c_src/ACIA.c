#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\ACIA.h"
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\lcd.h"
#include <string.h> 

// ***** Function to initialize the ACIA and set up speed, parity, etc. *****
// Needs to be done only once 
void ACIA_init(void) {
	ACIA[STAT] = 0x00; 		// writing anything to ACIA[STAT] will reset ACIA 
	// ACIA[CTRL] = (STOP1 | BITS8 | BAUD_GEN | BAUD115200); 	
	ACIA[CTRL] = 0x10;		// 1 stop / 8 data / 115.2K baudrate 
	// ACIA[CMD] = (NO_PARITY | NO_ECHO | RX_IRQ_DIS) ; 				
	ACIA[CMD] = 0x09; 		// no parity / no echo / interrupt on Rx register full 
}

// Function outputs one ASCII character via serial port to the console 
// Function waits for confirmation char was sent 
void ACIA_echo(char ascii) {
	// After loading an ASCII value in the data register, the ACIA sets the 
	// TX_EMPTY flag (=1) indicating data is being sent. Once all data was sent, 
	// TX_EMPTY flag will be reset (=0). 
	// The TX_EMPTY flag is located in the ACIA[STAT] register. 
	ACIA[DATA] = ascii;							// put ASCII code in data register 
	while (!(ACIA[STAT] & ACIA_TX_EMPTY));		// loop until TX_EMPTY is 1(=char sent)
	
	// testing of CA2 (VIA pin 39) 
	if (VIA2[PCR] & CA2_HIGH_OUT) {
		VIA2[PCR] |= CA2_HIGH_OUT; 	// make pin 39 high 
	}
	else {
		VIA2[PCR] |= CA2_LOW_OUT; 	// make pin 39 low 
	}
}

// Function outputs a welcoming banner message to the console 
// String(s) defining the msg are in MSG_TERM_WELC. Length must be <=255. 
void ACIA_bootscreen(void) {
	unsigned int cnt = 0; 
	ACIA_echo(asc_ESC); 					// "ESC [ 2 J" is "CLS" for VT100 
	ACIA_echo('['); 
	ACIA_echo('2'); 
	ACIA_echo('J'); 
	ACIA_echo(asc_ESC); 					// "ESC [ H" is "move cursor home" for VT100
	ACIA_echo('['); 
	ACIA_echo('H'); 
	while (MSG_TERM_WELC[cnt])  			// if not yet EOS 
		ACIA_echo(MSG_TERM_WELC[cnt++]); 	// output char and incr ctr 
}

// ***** Function to print a CR/LF combination followed by the command prompt on the console *****
// The prompt character used (typ. > or \ or % etc) is in CMD_PROMPT in the header file. 
void ACIA_prompt(void) {
	ACIA_echo('\r'); 						// output LF
	ACIA_echo('\n'); 						// output CR 
	ACIA_echo(CMD_PROMPT); 					// output prompt 
}


// ***** Function to read a char received via the serial port from the console *****
// Returns NULL if no char was received or the ASCII value if there was. 
char getchar(void) {
	// If WR_PTR = RD_PTR, the buffer has no unread characters and we return '\0'. 
	// If not, return the character at RD_PTR position and advance the RD_PTR.  
	// WR_PTR is advanced in the IRQ handler routine every time a char is received. 
	if (WR_IDX != RD_IDX)
		return INPUT_BUFFER[RD_IDX++]; 			// read char from circular buffer 
	else 
		return ('\0'); 							// return \0 or char from circ buffer 
}

// ***** Function to write a char via the serial port to the console *****
// Function overwrites the function with the same name in <stdio.h>
char putchar(char chr) {	// write a character to stdout (overriding the macro of the same name in stdio.h)
	ACIA_echo(chr); 
	return 1; 			// indicating one char was sent 
}

// ***** Function to write a string via the serial port to the console *****
// ***** Automatically appends a CR/LF at the end 
// Function overwrites the function with the same name in <stdio.h> 
void puts(const char *s) {
	// array name = address of 1st element, i.e. p_a=&a[0] and *p_a=a[0] 
	// this also means: p_a+i = &a[i] and *(p_a+i)=a[i]
	while (*s != '\0') 								// if not yet EOS 
	{												// output char and incr ctr 
		ACIA[DATA] = *(s++);						// put ASCII code to send in data register 
		while (!(ACIA[STAT] & ACIA_TX_EMPTY));		// loop/wait until TX_EMPTY is not 0 ('1'=char sent)
	}
	putchar('\r');
	putchar('\n'); 
}
// ***** Same as puts function but without the CR/LF appended *****
void putstring(const char *s) {
	while (*s != '\0') 
		ACIA_echo(*s++); 
}

//****** orignal version, using hardware directly, prior to using interrupts 
// ***** Function to read a char received via the serial port from the console *****
// Returns NULL if no char was received or the ASCII value if there was. 
// char getchar(void)
// { 	
	// read TX/RX buffer status and check if ACIA_RX_FULL bit is set.  
	// If set, a char was received, so return its ASCII value
	// If not set, no char received, so return NULL. 
	// if (ACIA[STAT] & ACIA_RX_FULL)
		// return ACIA[DATA]; 
	// else 
		// return '\0'; 
// }

// char uart_getchar(FILE *stream)
// {
	// if (ACIA[STAT] & ACIA_RX_FULL)
		// return ACIA[DATA]; 
	// else 
		// return '\0'; 
// }	

// char uart_putchar(char c, FILE *stream)
// {
	// putchar(c); 
// }	
