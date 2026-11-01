/*************************************************************************************************/
#include "C:\Users\DM\Documents\Datasheets\Microcontroller\65816\VSCode\c_include\RTC.h" 

void RTC_Set_Time(void) {
	char ctr, hrs, mins, secs, dow, mon, dom; 
	int	year; 
	char strng[15] = ""; 
	static const int offset[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	static const char months[13][3]={"   ", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; 
		//             01234567
		// __TIME__ : " 0:06:59"
	hrs  = (__TIME__[1]) - 0x30; 				// units digit is always there 
	if (' ' != __TIME__[0]) 					// if there is a tens digit 
		hrs += (__TIME__[0]-0x30)*10; 			
	mins = (__TIME__[3]-0x30)*10 + (__TIME__[4]-0x30); 	// mins = 35  
	secs = (__TIME__[6]-0x30)*10 + (__TIME__[7]-0x30); 	// secs = 19
		//            01234567890
		// __DATE__ : Jan 12 2026 
	dom = __DATE__[5]-0x30; 					// units digit
	if (' ' != __DATE__[4]) 
		dom += (__DATE__[4]-0x30)*10; 			// in case there is a tens digit
	year = (__DATE__[9]-0x30)*10 + (__DATE__[10]-0x30); 		// year = 26 
	for (ctr = 1; ctr<13; ctr++) {				// cycle through all months 
		if (0==strcmp(__DATE__, months[ctr]))	// if a match is found 
			mon=ctr;							// assign the month 
	} 
 		// Calculate DoW when given a day/month/year using the Tomohiko Sakamoto's algorithm 
		// Input ranges: day=1..31; month=1..12; year=20xx
		// Return range: Sun=0..Sat=6 
	year = year + 2000; 						// make year 20xx so we can use it in the dow calculation 
	year = year - (mon < 3);					// in full: if (mon<3) yr2k = yr2k - 1; 
	dow = (year + year / 4 - year / 100 + year / 400 + offset[mon - 1] + dom) % 7; 
	year = year - 2000 + (mon < 3); 			// make year suitable again for RTC format  
	ctr = RTC[RTC_SEC100]; 						// latch the time into registers 
	RTC[RTC_CMD_REG] = RTC_1M; 					// 1.048576 MHz crystal  
	RTC[RTC_HRS] 	 = hrs; 					// 0..23
	RTC[RTC_MINS] 	 = mins;        			// 0..59
	RTC[RTC_SECS] 	 = secs;        			// 0..59
	RTC[RTC_MTHS] 	 = mon;         			// 1..12
	RTC[RTC_DoMS] 	 = dom;         			// 1..31
	RTC[RTC_YEARS] 	 = year;          			// 0..99
	RTC[RTC_DoW] 	 = dow;         			// 0..6	
	RTC[RTC_ISMR]    = 0x00; 					// reset all interrupts 
	RTC[RTC_CMD_REG] = RTC_IE | RTC_RUN | RTC_24HR | RTC_1M; // 0x10+0x08+0x04+0x01
	RTC[RTC_ISMR]    = RTC_ISMR_SEC; 			// interrupt every sec 
} 

void RTC_Time_On_LCD(void) {
	char temp, hrs, min, sec, mon, dom, year, dow; 
	static const char DoW[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; 
	temp = RTC[RTC_SEC100]; 				// reading 1/100 sec register latches time 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	min  = RTC[RTC_MINS]; 					// 0..59
	sec  = RTC[RTC_SECS]; 					// 0..59
	mon  = RTC[RTC_MTHS]; 					// 1..12
	dom  = RTC[RTC_DoMS]; 					// 1..31
	year = RTC[RTC_YEARS]; 					// 0..99
	dow  = RTC[RTC_DoW]; 					// 0..6 = Sun..Sat
	// 012345678901
	// Mon 18:00:56 
	// Dec 23, 2025 
	instr_to_lcd(LCD_CLS); 
	lcd_set_cur_pos(0,0); 
	switch (dow) {
		case 0: 
			char_to_lcd('S'); char_to_lcd('u'); char_to_lcd('n'); char_to_lcd(' '); 
			break; 
		case 1: 
			char_to_lcd('M'); char_to_lcd('o'); char_to_lcd('n'); char_to_lcd(' '); 
			break; 
		case 2: 
			char_to_lcd('T'); char_to_lcd('u'); char_to_lcd('e'); char_to_lcd(' '); 
			break; 
		case 3: 
			char_to_lcd('W'); char_to_lcd('e'); char_to_lcd('d'); char_to_lcd(' '); 
			break; 
		case 4: 
			char_to_lcd('T'); char_to_lcd('h'); char_to_lcd('u'); char_to_lcd(' '); 
			break; 
		case 5: 
			char_to_lcd('F'); char_to_lcd('r'); char_to_lcd('i'); char_to_lcd(' '); 
			break; 
		case 6: 
			char_to_lcd('S'); char_to_lcd('a'); char_to_lcd('t'); char_to_lcd(' '); 
			break; 
	}
	char_to_lcd(hrs / 10 + 0x30);	char_to_lcd(hrs % 10 + 0x30);	char_to_lcd(':');
	char_to_lcd(min / 10 + 0x30);	char_to_lcd(min % 10 + 0x30);	char_to_lcd(':');
	char_to_lcd(sec / 10 + 0x30);	char_to_lcd(sec % 10 + 0x30);
	lcd_set_cur_pos(1,0); 
	// char_to_lcd(mon / 10 + 0x30);	char_to_lcd(mon % 10 + 0x30);	char_to_lcd('/');
	switch (mon) {
		case 0: 
			char_to_lcd('x'); char_to_lcd('x'); char_to_lcd('x'); char_to_lcd(' '); 
			break; 
		case 1: 
			char_to_lcd('J'); char_to_lcd('a'); char_to_lcd('n'); char_to_lcd(' '); 
			break; 
		case 2: 
			char_to_lcd('F'); char_to_lcd('e'); char_to_lcd('b'); char_to_lcd(' '); 
			break; 
		case 3: 
			char_to_lcd('M'); char_to_lcd('a'); char_to_lcd('r'); char_to_lcd(' '); 
			break; 
		case 4: 
			char_to_lcd('A'); char_to_lcd('p'); char_to_lcd('r'); char_to_lcd(' '); 
			break; 
		case 5: 
			char_to_lcd('M'); char_to_lcd('a'); char_to_lcd('y'); char_to_lcd(' '); 
			break; 
		case 6: 
			char_to_lcd('J'); char_to_lcd('u'); char_to_lcd('n'); char_to_lcd(' '); 
			break; 
		case 7: 
			char_to_lcd('J'); char_to_lcd('u'); char_to_lcd('l'); char_to_lcd(' '); 
			break; 
		case 8: 
			char_to_lcd('A'); char_to_lcd('u'); char_to_lcd('g'); char_to_lcd(' '); 
			break; 
		case 9: 
			char_to_lcd('S'); char_to_lcd('e'); char_to_lcd('p'); char_to_lcd(' '); 
			break; 
		case 10: 
			char_to_lcd('O'); char_to_lcd('c'); char_to_lcd('t'); char_to_lcd(' '); 
			break; 
		case 11: 
			char_to_lcd('N'); char_to_lcd('o'); char_to_lcd('v'); char_to_lcd(' '); 
			break; 
		case 12: 
			char_to_lcd('D'); char_to_lcd('e'); char_to_lcd('c'); char_to_lcd(' '); 
			break; 
	}
	char_to_lcd(dom / 10 + 0x30);	char_to_lcd(dom % 10 + 0x30);	
	char_to_lcd(',');					char_to_lcd(' ');
	char_to_lcd('2');				char_to_lcd('0');	
	char_to_lcd(year / 10 + 0x30);	char_to_lcd(year % 10 + 0x30);
}

void RTC_Time_On_Serial(void) {
	char temp, hrs, min, sec, mon, dom, year, dow; 
	static const char DoW[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; 
	temp = RTC[RTC_SEC100]; 				// reading 1/100 sec register latches time 
	hrs  = RTC[RTC_HRS]; 					// 1..12 or 0..23
	min  = RTC[RTC_MINS]; 					// 0..59
	sec  = RTC[RTC_SECS]; 					// 0..59
	mon  = RTC[RTC_MTHS]; 					// 1..12
	dom  = RTC[RTC_DoMS]; 					// 1..31
	year = RTC[RTC_YEARS]; 					// 0..99
	dow  = RTC[RTC_DoW]; 					// 0..6 = Sun..Sat
	// 012345678901
	// Mon 18:00:56 
	// Dec 23, 2025 
	switch (dow) {
		case 0: 
			putstring("Sun "); 
			break; 
		case 1: 
			putstring("Mon "); 
			break; 
		case 2: 
			putstring("Tue "); 
			break; 
		case 3: 
			putstring("Wed "); 
			break; 
		case 4: 
			putstring("Thu "); 
			break; 
		case 5: 
			putstring("Fri "); 
			break; 
		case 6: 
			putstring("Sat "); 
			break; 
	}
	putchar(RTC[RTC_HRS]  / 10 + 0x30); 	putchar(RTC[RTC_HRS]  % 10 + 0x30); 	putchar(':'); 
	putchar(RTC[RTC_MINS] / 10 + 0x30); 	putchar(RTC[RTC_MINS] % 10 + 0x30); 	putchar(':'); 
	putchar(RTC[RTC_SECS] / 10 + 0x30); 	putchar(RTC[RTC_SECS] % 10 + 0x30); 	putchar(' '); 
	switch (mon) {
		case 1: 
			putstring("Jan "); 
			break; 
		case 2: 
			putstring("Feb "); 
			break; 
		case 3: 
			putstring("Mar "); 
			break; 
		case 4: 
			putstring("Apr "); 
			break; 
		case 5: 
			putstring("May "); 
			break; 
		case 6: 
			putstring("Jun "); 
			break; 
		case 7: 
			putstring("Jul "); 
			break; 
		case 8: 
			putstring("Aug "); 
			break; 
		case 9: 
			putstring("Sep "); 
			break; 
		case 10: 
			putstring("Oct "); 
			break; 
		case 11: 
			putstring("Nov "); 
			break; 
		case 12: 
			putstring("Dec "); 
			break; 
	}
	putchar(RTC[RTC_DoMS] / 10 + 0x30); 	putchar(RTC[RTC_DoMS] % 10 + 0x30); 	
	putchar(','); putchar(' '); putchar('2'); putchar('0'); 
	putchar(RTC[RTC_YEARS] / 10 + 0x30); 	putchar(RTC[RTC_YEARS] % 10 + 0x30); 
	putchar('\r'); 							putchar('\n'); 
}

// functions to set individual bits of the RTC Command Register 
void RTC_24Hr_Mode(void) {
	// bit2 = 1 = 0bxxxxx1xx
	RTC[RTC_CMD_REG] |= RTC_24HR; 		// | 0b00000100
} 

void RTC_12Hr_Mode(void) {
	// bit2 = 0 = 0bxxxxx0xx
	RTC[RTC_CMD_REG] &= !(RTC_24HR); 		// & 0b11111011
} 

void RTC_RunMode (void) {
	// bit3 = 1 = 0bxxxx1xxx
	RTC[RTC_CMD_REG] |= RTC_RUN;			// | 0b00001000
}

void RTC_StopMode (void) {
	// bit3 = 0 = 0bxxxx0xxx
	RTC[RTC_CMD_REG] &= !(RTC_RUN); 		// & 0b11110111
}

void RTC_Enable_Interrupt(void) {
	// bit4 = 1 = 0bxxx1xxxx
	RTC[RTC_CMD_REG] |= RTC_IE; 		// | 0b00010000
} 

void RTC_Disable_Interrupt(void) {
	// bit4 = 0 = 0bxxx0xxxx
	RTC[RTC_CMD_REG] &= !(RTC_IE);	// & 0b11111011
} 

void RTC_in_Test_Mode(void) {
	// bit5 = 1 = 0bxx1xxxxx
	RTC[RTC_CMD_REG] |= RTC_TEST; 		// | 0b00100000
} 

void RTC_in_Normal_Mode(void) {
	// bit5 = 0 = 0bxx0xxxxx
	RTC[RTC_CMD_REG] &= !(RTC_TEST); // & 0b11011111
} 
