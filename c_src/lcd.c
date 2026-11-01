#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\lcd.h"

void check_for_lcd_ready(void) {	
// ***** Helper function to check LCD busy status.  *****
// Needed to ensure one instruction is completed prior to sending the next one. 
	// PORTB7..4 = 0 
	// EX=start read-write / RW=read-not write / RS=data/not instr register select
	// PORTB3..0 = D3 / D2 / D1 / D0 	
	// EX=0x40 ; RW=0x20 ; RS=0x10
	char hinib;
	char lonib;
	VIA2[DDRB] = 0xF0;						// set PORTB7..4 to outputs, PORTB3..0 to inputs 
											// B7=n/c; B6=EN; B5=RWB; B4=RS; B3..B0=data
	do {
		VIA2[PORTB] = RW;					// "read busy flag" instr = 0/EX=0/RW=1/RS=0
		VIA2[PORTB] = RW + EX;				// toggle EX to execute instr 
		hinib = VIA2[PORTB];				// read DB7..4 into PORTB3..0; save in hinib 
		VIA2[PORTB] = RW;					// RS=0/RW=1=read busy flag instr
		VIA2[PORTB] = RW + EX;				// toggle EX to execute instr
		lonib = VIA2[PORTB];				// read DB3..0 via PORTB3..0; save in lonib, not used
		VIA2[PORTB] = RW;					// RS=0/RW=1=read busy flag instr
	}										// hinib bit 3 is set when LCD is busy 
	while (hinib & LCD_BUSY_FLAG); 			// exit once DB7 is clear
	VIA2[DDRB] = 0xFF; 						// set all pins to output again
}

void char_to_lcd(char data) {
// ***** Write an ASCII code in the LCD's display data register at current cursor position *****
// "lcd_set_cur_pos(row, col)" function can/should be used to set cursor position first.   
	// PORTB7..4 = 0
	// EX=start read-write/RW=read-not write/RS=data/not instr register select
	// PORTB3..0 = D3/D2/D1/D0 and "data" payload is hhhhllll 
	// EX=0x40 ; RW=0x20 ; RS=0x10
	// "write data" requires PORTB5=RW=0=write and PORTB4=RS=1=data reg 
	VIA2[DDRB] = 0xFF; 								// all outputs 
	VIA2[PORTB] = (data >> 4) + RS; 				// high nibble (PORTB = 0001hhhh) and RS=1=data reg
	VIA2[PORTB] = (data >> 4) + RS + EX;			// set EX bit: PORTB is 0101hhhh
	VIA2[PORTB] = (data >> 4) + RS; 				// reset EX bit: PORTB is 0001hhhh
	VIA2[PORTB] = ((data << 4) >> 4) + RS; 			// PORTB is 0001llll; 
	VIA2[PORTB] = ((data << 4) >> 4) + RS + EX;		// set EX bit: PORTB is 0101llll
	VIA2[PORTB] = ((data << 4) >> 4) + RS;			// reset EX bit: PORTB is 0001llll
	check_for_lcd_ready();                  		// wait until busy flag is clear 
}

void instr_to_lcd(char instr) {
// ***** Function to send an "LCD instruction" to the LCD *****
// Instructions such as cls, home cursor, set #lines/#chars, ... 
	// PORTB7..4 = 0
	// EX=start read-write/RW=read-not write/RS=data-not instr register select
	// PORTB3..0 = D3/D2/D1/D0 and "instr" payload is hhhhllll 
	// "write instr" requires PORTB5=RW=0=write and PORTB4=RS=0=instr reg 
	VIA2[DDRB] = 0xFF; 							// all outputs 
	VIA2[PORTB] = (instr >> 4); 				// high nibble (PORTB = 0001hhhh) and RS=1=data reg
	VIA2[PORTB] = (instr >> 4) + EX;			// set EX bit: PORTB is 0101hhhh
	VIA2[PORTB] = (instr >> 4); 				// reset EX bit: PORTB is 0001hhhh
	VIA2[PORTB] = ((instr << 4) >> 4); 			// PORTB is 0001llll; 
	VIA2[PORTB] = ((instr << 4) >> 4)+EX;	 	// set EX bit: PORTB is 0101llll
	VIA2[PORTB] = ((instr << 4) >> 4);			// reset EX bit: PORTB is 0001llll
	check_for_lcd_ready();                  	// wait until busy flag is clear 
}

void lcd_init(void) {
// ***** Function to initialize the LCD, i.e. preparing it for use *****
// E.g. sets up 4bit mode, 2 lines, cursor on, blink on, cls, cursor home, etc. 
	// On VIA2: 
	//		NC/EX/RW/RS of LCD = PORTB7/6/5/4 (high nibble) 
	// 		DB7/6/5/4   of LCD = PORTB3/2/1/0 (low nibble) 
	// So every 8 bits of data need to be sent to the LCD in 2 sets of 4. 

	// "Function set" (001D NF**) with D=0 (4 bit), N=1 (2 lines), F=0 (5x8 font) = 0x28
	// After power up, LCD defaults to 8bit mode and DB3..0 are not physically connected, so lost. 
	// First time setting to 4 bits cannot change Number-of-Lines or Font. 
	// A second time may be required. 
	VIA2[DDRB] = 0xFF; 							// all outputs 
	VIA2[PORTB] = 0x02; 						// hinib = 0000 and lonib=0010
	VIA2[PORTB] = 0x42;							// set EX : hinib=0100 and lonib=0010
	VIA2[PORTB] = 0x02; 						// reset EX : hinib=0000 and lonib=0010

		// "Function set" (001D NF**) with D=0 (4 bit), N=1 (2 lines), F=0 (5x8 font) 
	instr_to_lcd(LCD_FUNC_SET|LCD_TWO_LINES); 	// 4bits/2 lines/5x8 = 001010xx = 0x28
		// "Entry mode set" (0000 01IS) with I=1 (incr), S=0 (displ shift off) 
	instr_to_lcd(LCD_ENTRY_MODE|LCD_INCR); 		// incr on/shift off = 00000110 = 0x06 
		// Displ on/off control = 0000 1DCB = 00001111 = 0x0F with D=1 (displ on), C=1 (cursor on), B=1 (blink on)  
	instr_to_lcd((LCD_DISPL_MODE|LCD_DISPL|LCD_CURSOR|LCD_BLINK)); 	// displ on/cursor on/blink on
	instr_to_lcd(LCD_CLS); 						// clear display = instruction "0x01" 
	instr_to_lcd(LCD_HOME); 					// cursor home = instruction "0x02"
}

void lcd_set_cur_pos(char row, char col) {
// ***** Function to set the cursor position to where the next char will be written ***** 
// A 2 line x 16 chars display, top row has addr 0x00..0x0F and bottom row 0x40..0x4F
	// "set DDRAM address" = 1RCCCCCC 
	// where R = row (0..1) and C = 0..15 = 0b000000..0b001111 
	if (0 == row) 
		instr_to_lcd(LCD_SET_DDRAM + col); 					// 0x80="set DDRAM addr" 
	else // row=1
		instr_to_lcd(LCD_SET_DDRAM + LCD_2ND_LINE + col); 	// add 0x40 for 2nd row addr
}

void lcd_boot_msg(void) {
// ***** Output a 2 line boot msg on the LCD  ***** 
// Strings for both lines are LCDMSG1 and LCDMSG2 in the header file 
	char cnt = 0; 
	lcd_set_cur_pos(0,0); 					// cursor to start of 1st line 
	while (LCD_Line1[cnt])
		char_to_lcd(LCD_Line1[cnt++]);		// write 1st line 
	lcd_set_cur_pos(1,0); 					// cursor to start of 2nd line 
	cnt=0;
	while (LCD_Line2[cnt])
		char_to_lcd(LCD_Line2[cnt++]);		// write 2nd line 
}
