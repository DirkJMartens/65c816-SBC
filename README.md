# 65c816-SBC
Wire wrapped SBC based on WDC W65C816SXB (i.e. 16-bit version of 6502): 

- Uses WDC W65C816SBX as a basic schematic
- See "https://github.com/DirkJMartens/65c816-SBC"
- Specifications:
  - CPU printed circuit board: 
    - 65C816 CPU, max 14 MHz clock frequency
    - TL7705: reset circuit and voltage supervisor for uC/uP systems
    - 32K RAM, 62256
    - 32K EEPROM, 28c256
    - 65C51 ACIA, 115.2KBaud serial connection
    - TTL-RS232 serial to USB convertor
  - DISPLAY/IO printed circuit board:
    - 8 hex-to-seven-segment display convertors to display content in HEX format 
    - Databus content shown in hex format on 2 seven-segment displays 
    - Memory address shown in hex format on 4 seven-segment displays 
    - Bank address shown in hex format on 2 seven-segment displays
    - Show the same in binary format on 4 LED bargraphs 
    - 65C22 VIA with 2 eight-bit I/O ports A and B 
    - Port A status is displayed on LED bar graph
    - Port B is connected to a 16 chars by 2 lines LCD display in 4-bit mode
- Initial build was a wire-wrapped version on 2 PCBs, fully tested and working.
  - Hardware built and tested during the year-end holiday season Dec 2023/Jan 2024
  - Software written over the next few months: Feb-Apr 2024 
    - Implemented and tested Wozmon
    - Wrote my own monitor program based on other people's ideas
- Converted to a 4 layer PCB: 
  - Consisting of 3 Eurocard-sized PCBs (100x160mm each or 4"x6.25")
  - Manufacturing was done as a 3-in-1 board of 303mm x 160mm board
  - Cut the 3-in-1 into its 3 individual PCBs: CPU, display and IO. 
  - Most of the original wirewrapped CPU board was retained. 
  - The Display/IO board was split into 2 separate ones 
  - The Display board could potentially be used for other 8-bit projects (6502, Z80?, 8086? ...)
  - The I/O board has an "experiment" area for future use with e.g. VDU chips, RTC chip ...
  - Additions and changes between wirewrap and PCB version include:
    - Added NMI button, DC power jack, power light ...
    - Used 74xx573 instead of 74x373 for better board routing 
    - LEDs for many control signals to visualize status of RAM, ROM, emul/nativ, R/W, ...
    - Used FTDI board for RS232-to-USB conversion
    - Changed all THT design to mixed THT/SMD (e.g. pull-ups, decoupling caps, LEDs ...) 
    - Various other minor changes 
- Possible future enhancements:
  - Add Teletext-based VDU based on SAA5020 and SAA5050 
  - Add a TMS9918-based video graphics adaptor
  - Add a Real Time Clock for date/time 
  - Build in 19-inch Euro-rack
- Based on / similar projects:
  - Adrian Kohlbecker (https://www.youtube.com/playlist?list=PLdGm_pyUmoII9D16mzw-XsJjHKi3f1kqT)
  - Dan Grise (https://www.youtube.com/watch?v=jgMxO3W5qBE)
  - TrackZero (https://www.youtube.com/watch?v=1spMekg8GM8&t=796s)
