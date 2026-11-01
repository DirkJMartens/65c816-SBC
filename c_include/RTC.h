/** @file RTC.h *********************************************************************************
*
* @brief Header file to use the Intersil ICM7170 RTC chip which provides a way 
*			to keep track of date and time, as well as periodic and programmed interrupts.
*			The RTC chip is connected to the lowest 5 address bits of the address bus. 
*			It also is connected to the 8 bit data bus. 
*			There are 3 control signals (RWB, WRB, ALE) driven by 3 LSBs of VIA2 port A. 
*			The RTC's CSB pin is connected to XCS0, which is mapped to addr 0x7F00..0x7F1F 
* @par
* COPYRIGHT NOTICE: (c) 2025 Dirk Martens. No right reserved. Use at your own risk. 
*/


// 				6502/65816 CPU                                       ICM7170 RTC
// 				+----------------+                                   +---------------+
// 				|     D0 - D7    |===================================|    D0 - D7    | Data Bus
// 				|                |                                   |               |
// 				|     A0 - A4    |-----------------------------------|    A0 - A4    | Address Bus
// 				|                |                                   |               |
// 				|  Address Dec.  |----[ Active-Low Select Signal ]---|      /CS      | Chip Select
// 				|                |                                   |               |
// 				|                |                                   |      ALE      | Tied to VCC (+5V)
// 				|                |                                   |               |
// 				|      R/W       |------+                            |               |
// 				|                |      |   |\                       |               |
// 				|                |      +---|O>----------------------+      /RD      | Read Strobe
// 				|                |      |   |/  (74HCT02 NOR)        |               |
// 				|      Φ2        |------+                            |               |
// 				|    (Clock)     |------+                            |               |
// 				|                |      |   |\                       |               |
// 				|                |      +---|O>----------------------+      /WR      | Write Strobe
// 				|                |          |/  (74HCT02 NOR)        |               |
// 				+----------------+                                   +---------------+


#ifndef RTC_H
#define RTC_H 

#define		RTC_BASE_ADDR	0x7F00 			// base addr of XCSOB = 0x7C00..0x7C1F
#define		RTC	((volatile char *)0x7F00) 

// definitions for the 17 internal RTC registers, all in binary format, 3 to 7 bits 
#define		RTC_SEC100		0x00 			// 1/100th of seconds (7 bits) 
#define		RTC_HRS			0x01 			// hours (1..12 or 0..23) (4 or 5 bits) 
#define		RTC_MINS		0x02 			// minutes (0..59) (6 bits) 
#define		RTC_SECS		0x03 			// seconds (0..59) (6 bits)  
#define		RTC_MTHS		0x04 			// month (1..12) (4 bits) 
#define		RTC_DoMS		0x05 			// day of month (1..31) (5 bits) 
#define		RTC_YEARS		0x06 			// year (0..99) (7 bits) 
#define		RTC_DoW			0x07 			// day of week (0..6, Sun..Sat) (3 bits) 
#define		RTC_RAM_SEC100	0x08 			// Alarm 1/100th of seconds 
#define		RTC_RAM_HRS		0x09 			// Alarm hours 
#define		RTC_RAM_MIN		0x0A 			// Alarm minutes 
#define		RTC_RAM_SEC		0x0B 			// Alarm seconds 
#define		RTC_RAM_MTH		0x0C 			// Alarm month 
#define		RTC_RAM_DATE	0x0D 			// Alarm day of month 
#define		RTC_RAM_YEAR	0x0E 			// Alarm year 
#define		RTC_RAM_DoW		0x0F 			// Alarm day of week 
#define		RTC_ISMR		0x10 			// Interrupt Status and Interrupt Mask register 
#define		RTC_CMD_REG		0x11 			// Command Register 

// Memory locations where date/time are accessible as individual digits (stored as DEC values)  
#define 	RTC_MEM_10HRS	0x12			// tens of hours  
#define 	RTC_MEM_HRS		0x13			// hours
#define 	RTC_MEM_10MINS	0x14			// tens of mins 
#define 	RTC_MEM_MINS	0x15			// mins 
#define 	RTC_MEM_10SECS	0x16			// tens of secs
#define 	RTC_MEM_SECS	0x17			// secs
#define 	RTC_MEM_10MONS	0x18			// tens of month
#define 	RTC_MEM_MONS	0x19			// month
#define 	RTC_MEM_10DOMS	0x1A			// tens of day of month 
#define 	RTC_MEM_DOMS	0x1B			// day of month 
#define 	RTC_MEM_10YRS	0x1C			// tens of years
#define 	RTC_MEM_YRS		0x1D			// years 
#define 	RTC_MEM_DOW		0x1E			// day of week 

// Command register, write only 
//          d7        d6        d5        d4       d3        d2      d1    d0 
//     +---------+---------+---------+---------+---------+---------+---------+
// 0 = |    0    |    0    | Normal  | Int Dis |  Stop   |  12Hr   |  Xtal   |
// 1 = |    0    |    0    |  Test   | Int Ena |  Run    |  24Hr   | speed   |
//     +---------+---------+---------+---------+---------+---------+---------+ 
#define		RTC_32K				0x00	// 0bxxxxxx00
#define		RTC_1M				0x01	// 0bxxxxxx01
#define		RTC_2M				0x02	// 0bxxxxxx10
#define		RTC_4M				0x03	// 0bxxxxxx11
#define		RTC_24HR			0x04	// 0bxxxxx1xx
#define		RTC_RUN				0x08	// 0bxxxx1xxx
#define		RTC_STOP			0xF7	// 0b11110111
#define 	RTC_IE				0x10	// 0bxxx1xxxx
#define		RTC_TEST			0x20	// 0bxx1xxxxx

//            d7        d6        d5        d4       d3        d2         d1        d0 
//       +---------+---------+---------+---------+---------+---------+---------+--------+
// Write |    NU   |   Day   |   Hour  |   Min   |  Sec    |1/10 Sec |1/100 Sec| Alarm  |
//       +---------+---------+---------+---------+---------+---------+---------+--------+ 
// interrupt and status register bit format. Only available when writing to ISMR. 
#define		RTC_ISMR_DAY		0x40	// 0b01000000
#define		RTC_ISMR_HR			0x20	// 0b00100000
#define		RTC_ISMR_MIN		0x10	// 0b00010000
#define		RTC_ISMR_SEC		0x08	// 0b00001000
#define		RTC_ISMR_SEC10		0x04	// 0b00000100
#define		RTC_ISMR_SEC100		0x02	// 0b00000010
#define		RTC_ISMR_ALM		0x01	// 0b00000001

//            d7        d6        d5        d4       d3        d2         d1        d0 
//       +---------+---------+---------+---------+---------+---------+---------+--------+
// Read  | Glb Int |   Day   |   Hour  |   Min   |  Sec    |1/10 Sec |1/100 Sec| Alarm  |
//       +---------+---------+---------+---------+---------+---------+---------+--------+ 
// interrupt and status register bit format. Only available when reading from ISMR. 
#define		RTC_ISMR_GINT		0x80	// 0b10000000
#define		RTC_ISMR_INTDAY		0x40	// 0b01000000
#define		RTC_ISMR_INTHR		0x20	// 0b00100000
#define		RTC_ISMR_INTMIN		0x10	// 0b00010000
#define		RTC_ISMR_INTSEC		0x08	// 0b00001000
#define		RTC_ISMR_INTSEC10	0x04	// 0b00000100
#define		RTC_ISMR_INTSEC100	0x02	// 0b00000010
#define		RTC_ISMR_INTALM		0x01	// 0b00000001

// forward declarations and prototypes of functions 
void RTC_Time_On_LCD(void); 
void RTC_Time_On_Serial(void); 
void RTC_Set_Time(void); 
void RTC_Xtal_32K(void); 
void RTC_Xtal_1M(void); 
void RTC_Xtal_2M(void); 
void RTC_Xtal_4M(void); 
void RTC_24Hr_Mode(void); 
void RTC_12Hr_Mode(void); 
void RTC_RunMode(void); 
void RTC_StopMode(void); 
void RTC_Enable_Interrupt(void); 
void RTC_Disable_Interrupt(void); 
void RTC_in_Test_Mode(void); 
void RTC_in_Normal_Mode(void); 
#endif   // end RTC_H