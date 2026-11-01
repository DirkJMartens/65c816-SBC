/** @file ACIA.h
*
* @brief Header file to use the UMC6551 ACIA chip.
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Dirk Martens. No right reserved. Use at your own risk. 
*/

#ifndef ACIA_H
#define ACIA_H 

// This is the hardware addresses the UMC6551 ACIA is mapped to. Change as needed. 
// The 'volatile' declaration ensures / forces the compiler uses a single byte read 
// instead of the more inefficient two byte read. 
// The ACIA registers can be accessed as ACIA[STAT], ACIA[CMD], etc. 
#define ACIA_BASE	0x7F80 
#define ACIA		((volatile char *)ACIA_BASE) 
#define DATA		(0) 				// stores send and receive data 
#define STAT		(1)					// contains Tx/Rx status, error status, overrun/overflow, etc 
#define CMD			(2)					// sets parity, interrupts used, etc 
#define CTRL		(3)					// sets baudrate, stop/start/word length. etc

// Buffered circular buffer (size = 256 bytes = 0xFF) for storing of ACIA Rx data 
// buffer defined and zero-initialized in ASM startup file 
#define INPUT_BUFFER 	((char *)0x1000) 
// #define INPUT_BUFFER 	((unsigned char *)0x0200) 

// STATUS REGISTER CONSTANTS  
/* 
	+---+---+---+---+---+---+---+---+
	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 
	+---+---+---+---+---+---+---+---+
	  |   |   |   |   |   |	  |	  |
	  |   |   |   |   |   |	  |	  +- Parity Error (bit 0) (self clearing)
	  |   |   |   |   |   |	  |			0 = No error / 1 = Error 
	  |   |   |   |   |   |	  +----- Framing error (bit 1) (self clearing)
	  |   |   |   |   |   |				0 = No error / 1 = Error
	  |   |   |   |   |   +--------- Overrun (bit 2) (self clearing)
	  |   |   |   |   |    				0 = No error / 1 = Error 
	  |   |   |   |   +------------- Receive Data Register Full (bit 3) (cleared by read RDR)
	  |   |   |   |        				0 = Not full/ 1 = Full 
	  |   |   |   +----------------- Transmit Data Register Empty (bit 4) (cleared by write TDR) 
	  |   |   |         				0 = Not Empty / 1 = Empty
	  |   |   +--------------------- DCDB (Data Carrier Detect) (bits 5) 
	  |	  |								0 = DCDB low / 1 = DCDB high 
	  |   +------------------------- DSRB (Data Set Ready) (bits 6) 
	  |									0 = DSRB low / 1 = DSRB high
	  +----------------------------- IRQ (bit 7) (Cleared by reading Status Register) 
										0 = No interrupt / 1 = Interrupt 
*/
#define IRQ_BIT 		(0x80)		// IRQ bit reflects IRQ (1=interrupt present) 
#define DSR_BIT 		(0x40)		// /DSR bit reflects /DSR state
#define DCD_BIT			(0x20)		// /DCD bit reflects /DCD state
#define ACIA_TX_EMPTY	(0x10)		// ACIA_STAT bit4 = Tx buffer empty/not empty 
#define ACIA_RX_FULL	(0x08)		// ACIA_STAT bit3 = Rx buffer full/not full 
#define OVERRUN_ERR		(0x04)		// Overrun error bit 
#define FRAME_ERR		(0x02)		// Framing error bit 
#define PARITY_ERR 		(0x01)		// Parity error bit 

// COMMAND REGISTER CONSTANTS 
/* 
	+---+---+---+---+---+---+---+---+
	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 
	+---+---+---+---+---+---+---+---+
	  |   |   |   |   |   |	  |	  |
	  |   |   |   |   |   |	  |	  +- Data Terminal Ready (bit 0) 
	  |   |   |   |   |   |	  |			0 = Disable receiver and all interrupts (DTRB high) 
	  |   |   |   |   |   |	  |			1 = Enable receiver and all interrupts (DTRB low) 
	  |   |   |   |   |   |	  +----- Receiver Interrupt Enable (bit 1)
	  |   |   |   |   |   |				0 = IRQB interrupt enabled from Status register bit 3
	  |   |   |   |   |   |				1 = IRQB interrupt disabled 
	  |   |   |   |   +---+--------- Transmitter Controls (bit 3 and 2) 
	  |   |   |   |        				0 0 = Tx interrupt disabled, RTSB high, Tx off 
	  |   |   |   |       				0 1 = Tx interrupt enabled, RTSB low, Tx on 
	  |   |   |   |      				1 0 = Tx interrupt disabled, RTSB low, Tx on 
	  |   |   |   |       				1 1 = Tx interrupt disabled, RTSB low, Tx transmit BRK 
	  |   |   |   +----------------- Normal/Echo mode for Receiver (bit 4) 
	  |   |   |         				0 = Normal (echo off) 
	  |   |   |         				1 = Echo on (bits 2 and 3 must be OFF ) 
	  +---+---+--------------------- Parity Check Controls (bits 7, 6, and 5) 
										- - 0 = Parity disabled, no parity bit generated / received 
										0 0 1 = Odd Parity Receiver and Transmitter 
										0 1 1 = Even Parity Receiver and Transmitter 
										1 0 1 = Mark parity Transmitted, parity check disabled 
										1 1 1 = Space parity Transmitted, parity check disabled 
*/
#define DTR_DIS			(0x00)		// DTRB high and all interrupt disabled (bit 0)
#define DTR_EN			(0x01)		// DTRB low and Receiver enabled (bit 0) 
#define RX_IRQ_EN		(0x00)		// Rx IRQ disabled (bit 1) 
#define RX_IRQ_DIS		(0x02)		// Rx IRQ enabled (bit 1) 
#define RX_EN			(0x01)		// Enable IRQ when RX data register full 
#define INT_OFF_TX_OFF 	(0x00)		// Tx IRQ disabled and RTSB high (Tx off) (bit 3 and 2) 
#define INTERR_DIS 		(0x04)		// Tx IRQ enabled and RTSB low (Tx On) (bit 3 and 2) 
#define INT_ON_TX_OFF	(0x08)		// Tx IRQ disabled and RTSB low (Tx On) (bit 3 and 2) 
#define NO_INT_TX_BRK	(0x0B)		// Tx IRQ disables and RTSB low (Tx BRK) (bit 3 and 2) 
#define NO_ECHO			(0x00)		// Receiver - No Echo Mode (bit 4)
#define ECHO			(0x10)		// Receiver - Echo Mode (bit 4) 
#define NO_PARITY 		(0x00)		// Parity disabled - No parity generated/received (bit 7,6,5)
#define ODD_PARITY		(0x20)		// Odd parity transmit and receive (bit 7,6,5)
#define OVEN_PARITY 	(0x60)		// Even parity transmit and receive (bit 7,6,5)
#define MARK_PARITY 	(0xA0)		// Mark parity bit transmitted, parity check disabled (bit 7,6,5)
#define SPACE_PARITY 	(0xE0)		// Space parity bit transmitted, parity check disabled (bit 7,6,5)

// CONTROL REGISTER CONSTANTS 
/* 
	+---+---+---+---+---+---+---+---+
	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 
	+---+---+---+---+---+---+---+---+
	  |   |   |   |   |   |	  |	  |
	  |   |   |   |   +---+---+---+- Baud Rate Generator (bits 3, 2, 1, and 0) 
	  |   |   |   |        				0 0 0 0 = external clock / 16 
	  |   |   |   |       				0 0 x x = 50 / 75 / 110 
	  |   |   |   |        				0 1 0 0 = 135 / 150 / 300/ 600 
	  |   |   |   |        				1 0 0 0 = 1200 / 1800 / 2400 / 3600 
	  |   |   |   |        				1 1 0 0 = 4800 / 7200 / 9600 / 19200 
	  |   |   |   +----------------- Receiver Clock Source (bit 4) 
	  |   |   |         				0 = External Receiver Clock  
	  |   |   |         				1 = Baud Rate Generator 
	  |   +---+--------------------- Data Word Length (bits 6 and 5) 
	  |									0 0 = 8 bits 
	  |									0 1 = 7 bits
	  |									1 0 = 6 bits 
	  |									1 1 = 5 bits
	  +----------------------------- Stop Bits (bit 7) 
										0 = 1 stop bit 
										1 = 2 stop bits 
											1 stop bit if word length = 8 bits and parity 
											1.5 stop bits  if word length = 5 bits and no parity 
*/
#define STOP2			(0x80)		// 2 STOP BITS
#define STOP1			(0x00)		// 1 STOP BITS
#define BITS5			(0x60)		// DATA WORD LENGTH = 5 bits / word
#define BITS6			(0x40)		// DATA WORD LENGTH = 6 bits / word
#define BITS7			(0x20)		// DATA WORD LENGTH = 7 bits / word 
#define BITS8			(0x00)		// DATA WORD LENGTH = 8 bits / word 
#define BAUD_GEN		(0x10)		// RECEIVER CLOCK RATE - transmitter based
#define EXT_CLOCK		(0x00)		// RECEIVER CLOCK RATE - xtal based 
#define BAUD19200		(0x0F)		// BAUD RATES - 19200 baud
#define BAUD9600		(0x0E)		// BAUD RATES - 9600 baud
#define BAUD4800		(0x0C)		// BAUD RATES - 4800 baud
#define BAUD2400		(0x0A)		// BAUD RATES - 2400 baud
#define BAUD115200		(0x00)		// BAUD RATES - 115200 baud

// ASCII codes 
#define asc_NL			(0x00)		// Null char 
#define asc_BEL			(0x07)		// bell / beep 
#define asc_BS          (0x08)		// Backspace key 
#define asc_HT			(0x09)		// Horizontal tab 
#define asc_LF			(0x0A)		// Line feed 
#define asc_FF			(0x0C)		// Form feed 
#define asc_CR          (0x0D)     	// Carriage Return
#define asc_ESC         (0x1B)     	// ESC
#define asc_BLANK		(0x20)		// Space 
#define asc_DEL			(0x7F)		// DEL
// 	0x30 .. 0x39 = digits 0 thru 9 
//  0x41 .. 0x5A = Upper-case A .. Z 
//  0x61 .. 0x7A = Lower-case a .. z 

// Misc 
#define CMD_PROMPT		'>'			// command prompt 

// Read and Write pointers into the console circular buffer 
// both initialized to 0 in ASM code 
volatile char RD_IDX = 0; 
volatile char WR_IDX = 0; 

// forward declarations and prototypes
void ACIA_init (void); 				// initializes the ACIA for use 
void ACIA_echo (char);				// output a char to the console 
void ACIA_bootscreen (void);		// show above MSG_TERM_WELC on console 
void ACIA_prompt (void);			// shows command prompt on console 
char getchar (void); 				// get character from console 
char putchar (char); 				// write a character to console 
void puts (const char *);					// write a string to console with cr/lf 
void putstring (const char *s); 			// write a string to console (no cr/lf) 

#endif	// end ACIA_h 
