/** @file LCD.h
*
* @brief Header file to use the Synertek 6522 VIA2 chip which drives an
*			LCD display of 2 lines of 16 chars, driven in 4-bit mode.
*			We need 4 databits and 3 control lines, using PORTB[0..6] for this. 
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Dirk Martens. No right reserved. Use at your own risk. 
*/

#ifndef LCD_H
#define LCD_H 

// An alternative method to use VIA registers is by defining a struct 
// This is shown in the file 6522.h and 6522.c 

// This is the hardware addresses the Synertek 6522 VIA is mapped to. 
// Change according to your needs. 
// The 'volatile' declaration ensures / forces the compiler uses
// a single byte read instead of the more inefficient two byte read
// The VIA registers can be accessed as VIA2[STAT], VIA2[CMD], etc. 
#define		VIA_BASE_ADDR		0x7FE0 

#define		VIA2 	((volatile char *)VIA_BASE_ADDR)
#define		ORB		0		// output register PORTB
#define		IRB		0		// input register PORTB - synonym for prev 
#define		PORTB	0		// PORTB - synonym for prev two
#define		ORA		1		// output register PORTA
#define		IRA		1		// input register PORTA - syn
#define		PORTA	1		// PORTA - synonym for prev two
#define		DDRB	2		// data direction register PORTB
#define		DDRA	3		// data direction register PORTA 
#define		T1CLO	4		// Timer 1 counter/latch low byte 
#define		T1CHI	5		// Timer 1 counter high byte 
#define		T1LLO	6		// Timer 1 latch low byte 
#define		T1LHI	7		// Timer 1 latch high byte 
#define		T2CLO	8		// Timer 2 
#define		T2CHI	9		// 
#define		SR		10		// Shift register 
#define		ACR		11		// Auxiliary control register 
#define		PCR		12		// Peripheral control register 
#define		IFR		13		// Interrupt Flag register 
#define		IER		14		// Interrupt Enable register
#define		ORANH	15		// Output register PORTA No Handshake 
#define		IRANH	15		// Input register PORTA No Handshake 
	
// *******************************************************************************
// 2 rows x 16 columns LCD - used in 4 bit mode, connected to PORTB bits 0 thru 6
//	PORTB7 = n/c; PORTB6 = EN; PORTB5 = Read/WriteB; PORTB4 = RegisterSelect 
//	PORTB3 ... PORTB0 = DB7..DB4 of the LCD; DB3..0 are not connected 
// *******************************************************************************
#define	EX 				0x40	// b6 = enable pin = 1 to start data read/write instruction
#define	RW 				0x20	// b5 = 1/0 = read/not write 
#define	RW_EX 			0x60	// b5 and 6 on
#define	RS 				0x10	// b4 = register select (0 = instr reg / 1 = data reg) 
								// b3..0 = 4 databits DB7..DB4 of LCD display
#define	LCD_CLS			0x01	// lcd function to clear screen 
#define	LCD_HOME		0x02	// lcd function to return cursor to topleft position 
#define	LCD_ENTRY_MODE	0x04	// lcd function to set cursor direction and display shift 
#define	LCD_INCR		0x02	// bit1 set for auto-incr position 
#define	LCD_DISPL_MODE 	0x08	// lcd function to turn display/cursor/blinking on/off 
#define	LCD_DISPL		0x04	// bit2 set for display on, clear for display off
#define	LCD_CURSOR		0x02	// bit1 set for cursor on, clear for cursor off
#define	LCD_BLINK		0x01	// bit0 set for blinking cursor on, clear for off 	
#define	LCD_FUNC_SET	0x20	// lcd function to set data length, display lines and font 
#define	LCD_TWO_LINES	0x08	// bit3 set for 2 lines, clear for 1 line 
#define	LCD_SET_CGRAM	0x40	// lcd function to set char generator RAM address 
#define	LCD_SET_DDRAM	0x80	// lcd function to set data display RAM address 
#define	LCD_2ND_LINE	0x40	// lcd function to set data display RAM address 
#define	LCD_NEXT_ROW	0x40	// on a 16x2 lcd, addr range is $00..$0F and $40..$4F resp. 
#define	LCD_BUSY_FLAG	0x08	// in 4bit mode, DB3 returns "busy flag" status 

// *******************************************************************************
// Constants the Periph Ctrl Reg to control the CA1 and CA2 output pins (pin40/39) 
// *******************************************************************************
#define CA1_NEGINTCTRL	0x00	// CA1 (VIA pin 40) Neg Edge Interrupt 
#define CA1_POSINTCTRL	0x01	// CA1 (VIA pin 40) Pos Edge Interrupt 
#define CA2_NEGACTINP	0x00	// CA2 (VIA pin 39) Input Neg Active Edge 
#define CA2_NEGACTINDINP 0x02	// CA2 (VIA pin 39) Independent Neg Edge Interrupt
#define CA2_POSACTINP	0x04	// CA2 (VIA pin 39) Positive Act Edge Input 
#define CA2_POSACTINDINP 0x06	// CA2 (VIA pin 39) Independent Pos Edge Interrupt
#define CA2_HANDOUT		0x08	// CA2 (VIA pin 39) Handshake output 
#define CA2_PULSEOUT	0x0A	// CA2 (VIA pin 39) Pulse output 
#define CA2_LOW_OUT		0x0C	// CA2 (VIA pin 39) Low Output 
#define CA2_HIGH_OUT	0x0E	// CA2 (VIA pin 39) High Output 

extern const char LCD_Line1[];	// defined in mystrings.h
extern const char LCD_Line2[];	// defined in mystrings.h

// forward declarations and prototypes
void check_for_lcd_ready (void);			// used internally by instr_to_lcd and char_to_lcd 
void char_to_lcd (char);			// send char data, e.g. ASCII or VT100 ESC codes 
void instr_to_lcd (char);			// send an instruction, e.g. displ size, font, etc. 
void lcd_init (void);						// initializes lcd for use 
void lcd_set_cur_pos (char, char);	// set cursor position 
void lcd_boot_msg (void);					// print LCDMSG1 and 2 on the lcd screen 

#endif   // end LCD_H