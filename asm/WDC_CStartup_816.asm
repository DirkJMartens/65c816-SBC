; *******S		; File: WDC_CStartup_816.asm
; 10/07/2003

    ; PW 132         ;Page Width (# of char/line) 
    ; PL 60          ;Page Length for HP Laser
    ; INCLIST ON     ;Add Include files in Listing

	;*********************************************
	;Test to ensure USING_816 is used in -D option
	;*********************************************
	; IFFALSE USING_816
		; EXIT	"Not Valid Processor: Use -DUSING_816 !"
	; ENDIF

	;*********************************************
	;Test to ensure SMALL is used in -D option
	;*********************************************
	; IFFALSE SMALL
		; EXIT	"Not using SMALL memory model: Use -DSMALL !"
	; ENDIF

	;****************************************************************************
	; End of testing for proper Command line Options for Assembly of this program
	;****************************************************************************
	; IF USING_816
		; IF SMALL
			; title  "WDC 'C' Startup Program V 1.01 for Small Model W65C816 - WDC_CStartup_816.asm"
		; ENDIF
	; ENDIF
; *******E

.TWOCHAR	ON				; enable use of CR, LF, NL, HT, SP mnemonics 

STACK   	EQU   	$7DFF   ; Top of Stack, near top of RAM. Grows towards smaller addresses. 
SP_FGND		EQU		$7000	; SP location to save main SP ToS (3 bytes)
SP_ISR		EQU		$7002	; SP location to save ISR SP ToS (3 bytes)

ACIA_DATA	EQU		$7F80	; ACIA DATA REGISTER (Tx and Rx data register) 
ACIA_STAT	EQU		$7F81	; ACIA STATUS REGISTER (Err bits, TxDR, RxDR, DCDB, DSRB, IRQ) 
ACIA_CMD	EQU		$7F82	; ACIA COMMAND REGISTER (DTR, RIE, TxC, Echo, Parity) 
ACIA_CTRL	EQU		$7F83	; ACIA CONTROL REGISTER (Baudrate, Data length, Stop bits) 

PORTB		EQU		$7FE0	; I/O port B on VIA2
PORTA		EQU		$7FE1	; I/O port A on VIA2
DDRB		EQU		$7FE2	; data direction register B on VIA2 (1=out; 0=in)
DDRA		EQU		$7FE3	; data direction register A on VIA2 (1=out; 0=in)
 
STL         EQU 	$26		;mem addr low 
STH         EQU 	$27		;mem addr high 
L           EQU		$28		;ASCII to HEX value low 
H           EQU 	$29		;ASCII to HEX value high 
COUNTER     EQU 	$2E		;keeps track of # ASCII chars processed  
CRC         EQU 	$2F		;keeps track of running CRC checksum
CRCCHECK   	EQU 	$30		;flag, set when CRC error 
IN          EQU 	$1000   ;Input buffer

; ASM macros to e.g. switch between 8 and 16 bit registers 
; mostly definitions for the WDC assembler 
; *******S
R16 .MACRO			; A and X/Y both 16 bits 
	LONGA ON
	LONGI ON
	REP #$30
	.ENDM
R8 .MACRO			; A and X/Y both 8 bits 
	LONGA OFF
	LONGI OFF
	SEP #$30
	.ENDM
A16 .MACRO			; A now 16 bits 
	LONGA ON
	REP #$20
	.ENDM
A8 	.MACRO			; A now 8 bits 
	LONGA OFF
	SEP #$20
	.ENDM
I16 .MACRO			; X/Y now 16 bits 
	LONGI ON
	REP #$10
	.ENDM
I8 	.MACRO			; X/Y now 8 bits 
	LONGI OFF
	SEP #$10
	.ENDM
; *******E

; INPUT_BUFFER and RD_IDX and WR_IDX are defined in ACIA.H
INPUT_BUFFER: SECTION 		
	; mapped to $1000 using "AINPUT_BUFFER=1000" in hex_file.opt file 
	BLKB $100,$0			; Console circular input buffer of 256 bytes; $1000 .. $10FF 
							; BLKB X,Y = define a block of X bytes, initialized with Y 

; BUFFER_IDX: SECTION 		; OFFSET $1100 
; RD_IDX	DSB	1				; RD_IDX is stored at $1100
; WR_IDX	DSB	1				; WR_IDX is stored at $1101 

STARTUP: SECTION 
; Prefixes define memory model used: Small : __ / Medium : ~_ / Compact : _~ / Large : ~~
	IF SMALL
	; This defines "START" as a label that the linker will fill in the address for. 
	; Defined as "extern void START(void);" in a C program 
		XDEF __START
	__START:
	ENDIF

E_RESET:
; *******S		; execution jumps here when a hardware reset is performed 
; mapped to $FE00 using "ASTARTUP=FE00" in hex_file.opt file 
		SEI					; 78: SEt Interrupt disable (=disable interrupts). 
							; "cli" in C_main enables interrupts 
        CLC                	; 18: clear carry 
        XCE                	; FB: clear emulation bit - in Native 16 bit Mode 
		CLD					; D8: clear decimal mode  

		; SEP		#$30		; 8 bit register A/X/Y
		; LONGI	OFF			; and tell assembler about it 
		; LONGA	OFF

		; init circular input buffer 
		; STZ		RD_IDX		; read pointer to SoB 
		; STZ		WR_IDX		; write pointer to SoB 

        REP   	#$30		; C2 30: 16 bit registers A/X/Y 
        LONGI 	ON			; and tell assembler about it 
        LONGA 	ON

        LDA   	#STACK      ; A9 FF 7D: get the stack address 
        TCS                	; 1B: and set the stack pointer to it 
; *******E

; *******S		; setup the databank register
		SEP #$20			; E2 20: A register 8 bit 
		LONGA OFF			; tell assembler 
		LDA #^_BEG_DATA		; A9 00: get bank part of DATA section 
		PHA					; 48: push it onto stack  
		PLB					; AB: and pull it into data bank register 
		REP #$20			; C2 20: A register 16 bit 
		LONGA ON 

; The assembler creates 3 symbols for each (user-defined) section: 
; 		_ROM_BEG_secname 	: this is the ROM location of the section 
; 		_BEG_secname 		: this is the execution beginning of the section 
; 		_END_secname		: this is the end of the section 
;			E.g. _ROM_BEG_STARTUP, _BEG_STARTUP and _END_STARTUP 
; 5 sections are always automatically created: 
; 		PAGE0 : for 6502: $0000 to $00FF; for 65816: $0000 to $FFFF (must be RAM)
; 		CODE : main program section (typ. in ROM) 
;		KDATA : constant data that never changes (e.g. constant strings; typ. in ROM)
; 		DATA : initialized data that will be modified (e.g. int cnt=0; cnt++, typ. RAM) 
;		UDATA : data that is not initialized (e.g. int temp; must be in RAM)
; The user can define up to 250 additional ones using "name: SECTION" 

; These symbols can be used by the programmer in the code to access, read, copy ... 

; Copy (initialized) DATA from ROM to RAM 
; 		assumes that DATA has been linked to reside in RAM but is
; 		physically in ROM starting at location _ROM_BEG_DATA 
; 		comment out this section if DATA does not need to be copied from ROM to RAM 

        LDA   #_END_DATA-_BEG_DATA 			; A9 B2 01: number of bytes to copy
					; BEG_DATA ~ 0x2000 and END_DATA ~ 0x21B2 
        BEQ   SKIP         					; F0 0A: if none, skip rest and jump to SKIP
        DEC   A            					; 3A: less one for MVN instruction
        LDX   #<_ROM_BEG_DATA 				; A2 00 20: low 16 bits of source (ROM) addr into X
        LDY   #<_BEG_DATA  					; A0 00 20: low 16 bits of dest (RAM) addr into Y
        MVN   #^_ROM_BEG_DATA,#^_BEG_DATA 	; 54 00 00: copy bytes using the bank parts of ROM/RAM 
; *******E

SKIP:
; fill the uninitialized UDATA area with zeroes 
; UDATA is uninitialized, so it does not need to be in ROM and also not copied to RAM 
; *******S
		LDX   #_END_UDATA-_BEG_UDATA  		; A2 02 00: get number of bytes to clear
					; BEG_UDATA ~ 0x21B2 and END_UDATA ~ 0x21B4 
        BEQ   DONE         					; F0 11: if none, skip rest and jump to DONE 
        LDA   #0           					; A9 00 00: load the zero to clear UDATA section in A reg 
        SEP   #$20       					; E2 20: A register 8 bits, so do byte at a time
        LDY   #_BEG_UDATA					; A0 B2 21: get beginning of UDATA into Y 
						
LOOP    STA   |0,Y       					; 99 00 00: clear memory using the 0 from A register 
        INY              					; C8: bump pointer
        DEX              					; CA: decrement count
        BNE   LOOP       					; D0 F9: loop till done
        REP   #$20       					; C2 20: 16 bit memory reg
DONE:
; *******E

; transfer control to the main() function in the C program 
	IF SMALL
; XREF = symbol declared outside this file (declare = name + type specified, no value assigned) 
; XDEF = symbol defined in this file (i.e. value assigned) but referenced in another file 
		XREF __main     ; jumps to "main" function of C program  
						; XREF tells assembler that the linker will fill in the address 
						; declare it as "extern void START (void);" in the file with main() 
						; and update reset vector accordingly 
		JSR >__main    	; 22 B3 97 00: using '>' to force long jump (in case not bank 0) (changes JSR into JSL) 
		BRK				; 00 00
	ENDIF

; declare special linker symbols for the linker to fill in 
; XREF = symbol declared outside this file (declare = name + type specified, no value assigned) 
	XREF  _BEG_DATA
	XREF  _END_DATA
	XREF  _ROM_BEG_DATA
	
	XREF  _BEG_UDATA
	XREF  _END_UDATA


ISRS: SECTION
;interrupt processing routines
; mapped at $FF00 using "ISRS=FF00" in .bat file 
; *******S
; Co-processor (not used) 
N_COP:							; 65816 mode 
E_COP:							; 6502 emulation mode 
		NOP						; EA: NOP 
		BRA N_COP				; 80 FD: endless loop of NOP

; Break (BRK s/w instruction) 
N_BRK:							; only used in 65816 mode
		NOP						; EA: NOP
		BRA N_BRK				; 80 FD: endless loop of NOP

; Abort (CPU pin 3) 
N_ABORT:						; 65816 mode
E_ABORT:						; 6502 emulation mode
		NOP						; EA: NOP
		BRA E_ABORT				; 80 FD: endless loop of NOP

; NMI (CPU pin 6 - frontpanel pushbutton)  
N_NMI: 							; 65816 mode 
E_NMI: 							; 6502 emulation mode 
		NOP						; EA: NOP 
		BRA E_NMI				; 80 FD: endless loop of NOP

; 
Undefined_IRQ: 					; 6502 and 65816 mode 
		NOP						; EA: NOP 
		BRA Undefined_IRQ		; 80 FD: endless loop of NOP

; XDEF = symbol defined in this file (i.e. value assigned) but referenced in another file 
; ISR to handle Wired-OR connections of IRQ pins of ACIA, VIA, ... 
; IRQPinHandler is defined in C_TEMPLATE_816.C 
 	XDEF __IRQPinHandler 
; *******E

;string constants table 
STRINGS: SECTION 
; mapped at $F900 using "ASTRINGS=F900" in bin_file.opt file 
; *******S
MSGCLS	DB $1B,'[','2','J','NL'
MSGHOM	DB $1B,'[','H','NL'
;LDPRG 	DB "Loading Intel HEX program ... ", 'CR', 'LF', 'NL'
LDPRG 	DB "Load ... ", 'CR', 'LF', 'NL' 
CRCERR	DB 'CR', 'LF', "Intel HEX file loaded with CRC error.", 'CR', 'LF', 'NL'
CRCOK	DB 'CR', 'LF', "Intel HEX file loaded successfully.", 'CR', 'LF', 'NL' 
LDDONE	DB 'CR', 'LF', "... end of file loading", 'CR', 'LF', 'NL'
NEWLINE DB 'CR', 'LF', 'NL'

SIGNATURE_DMA_SRC	DB "(C) by Dirk Martens " 	; Human readable
SIGNATURE_DMA_ASM	DB "C( )ybD ri kaMtesn  " 	; Little Endian format
					DATE						; Compilation Date/Time stamp
; *******E

;shadow vector table 
SHADOW: SECTION 		
; mapped at $7EE0 using "ASHADOW=7EE0" in bin_file.opt file 
; *******S
	; ; mapped 16 words of RAM 7EE0 ... 7EFF, just before memory mapped I/O
	; ;Status bit E = 0 (Native mode)
	; ds	4				; $7EE0 - Reserved(816)
						; ; $7FE2 - Reserved(816)
	; dw	Undefined_IRQ	; $7FE4 - COP(816)
	; dw	Undefined_IRQ	; $7FE6 - Brk(816)
	; dw	Undefined_IRQ	; $7FE8 - Abort(816)
	; dw	N_NMI			; $7FEA - NMI(816)
	; ds	2				; $7FEC - Reserved(816)
	; dw	__IRQHandler	; $7FEE - IRQ(816)
	; ;Status bit E = 1 (Emulation mode)
	; ds	4				; $7FF0 - Reserved(02)
						; ; $7FF2 - Reserved(02)
	; dw	Undefined_IRQ	; $7FF4 - COP(02Emulation)
	; ds	2				; $7FF6 - Reserved(02Emulation)
	; dw	Undefined_IRQ	; $7FF8 - Abort(02Emulation)
	; dw	E_NMI			; $7FFA - NMI (ALL)
	; dw	E_RESET			; $7FFC - Reset (ALL)
	; dw	__IRQHandler	; $7FFE - IRQBRK (ALL)
; *******E

;interrupt vector table
VECTORS: SECTION		
; mapped at $FFE0 using "AVECTORS=FFE0" in .bat file 
; *******S
	; the real reset and interrupt vectors

; replace Undefined_IRQ with actual interrupt routine name 
; they can only be 16 bits wide, so the IRQ routine must be in bank 0

; If not in bank 0, replace Undefined_IRQ with label "JMP0" and 
;	put in ORG directive 
; 		ORG xxxxx 
; before the label and long jump instruction 
; 		JMP0 	JMP >FUNC0 
; and then write the FUNC0 interrupt handler function 
; where ">" indicates long (=16 bit) pointers 

	; 65C816 Vectors
	; Status bit E = 0 (Native mode)
	ds	4				; $FFE0,1 - Reserved 
						; $FFE2,3 - Reserved
	dw	N_COP			; $FFE4,5 - Co-Processor 	(65816 mode)
	dw	N_BRK			; $FFE6,7 - Brk 			(65816 mode)
	dw	N_ABORT			; $FFE8,9 - Abort 			(65816 mode - pin 3)
	dw	N_NMI			; $FFEA,B - NMI 			(65816 mode - pin 6)
	ds	2				; $FFEC,D - Reserved 		(65816 mode)
	dw	__IRQPinHandler	; $FFEE,F - IRQ 			(65816 mode - pin 4)

	; 6502 Vectors
	; Status bit E = 1 (Emulation mode)
	ds 	4				; $FFF0,1 - Reserved 		(6502 mode)
						; $FFF2,3 - Reserved 		(6502 mode)
	dw	E_COP			; $FFF4,5 - Co-Processor 	(6502 mode)
	ds	2				; $FFF6,7 - Reserved 		(6502 mode)
	dw	E_ABORT			; $FFF8,9 - Abort 			(6502 mode - pin 3)
	dw	E_NMI			; $FFFA,B - NMI 			(6502 mode - pin 6)
	dw	E_RESET			; $FFFC,D - Reset 			(6502 mode - pin 40)
	dw	__IRQPinHandler	; $FFFE,F - IRQ/BREAK 		(6502 mode - pin 4)
; *******E

ends
end

; not used or temporarily disabled 
; *******S 

; *******S 
;ASM version of subroutine to process IRQ when ACIA receives data 
; replaced by C function " interrupt void IRQHandler(void); " in C_TEMPLATE_816.c
; Every time a character is received by the ACIA, an IRQ interrupt is generated and 
; this IRQ handler routine is run, putting the ASCII code of the char in the circular buffer. 
; The WR_PTR is used as the destination index into the circular buffer, incremented each 
; time a new interrupt is generated and wrapping around after 256 chars received. 
; The C main function reads the characters from the buffer and processes them. 
; !!! If not processed on time, the WR_PTR could wrap around and overwrite unprocessed chars.
; Modifies: CPU flags, A, X, Y 
; N_IRQ: 
		; REP #$30 			; 16 bit register A/X/Y so we save A/B and MSB/LSB of X/Y 
		; LONGA ON 			; and tell assembler about it 
		; LONGI ON 			; 
		; PHA					; save A/B, X and Y registers 
		; PHX					; since interrupt handler 

							; If there are multiple IRQ sources, we need to check which 
							; source generated the IRQ so we can handle it a dedicated 
							; handler routine. We have only one source, so we can skip. 

		; LDA ACIA_DATA		; char in RX DATA REGISTER into A register 
		; LDX WR_IDX		; offset for destination in circular buffer 
		; STA $1000,X		; save char into circular buffer 
		; INC WR_IDX		; prep offset for next char 

		; REP #$30 			; st A/X/Y back to 16 bit sizes 
		; LONGA ON 			; and tell assembler about it 
		; LONGI ON 			
		
		; PLX					; restore X and A/B registers 
		; PLA

		; RTI 				; return from interrupt
; *******E

; :10010000214601360121470136007EFE09D2190140
; :100110002146017E17C20001FF5F16002148011928
; :10012000194E79234623965778239EDA3F01B2CAA7
; :100130003F0156702B5E712B722B732146013421C7
; :00000001FF

; :06 5000 00 01 02 03 04 05 06 95
; :00 0000 01 FF

; :01 5000 00 01 AE
; :00 0000 01 FF

;NMI IRQ interrupt routine - used to read HEX file 
; *******S
; Subroutine to output null-terminated string 
; 	X reg contains 16-bit start addr of string 
; this compiles to ~FF3D
; PRINTMSG	I8				;8bit X/Y 
			; A8				;8bit A 
; ?NEXT      	LDA !0,X		;get Xth char 
			; BEQ ?DONE		;branch if null char reached 
			; STA ACIA_DATA	;output char to send (ACIA_DATA=$7F80)
; ?WAIT: 
			; LDA ACIA_STAT	;get send status (ACIA_STAT=$7F81)
			; AND #$10		;bit4=1 @ send complete
			; BEQ ?WAIT		;loop until ACIA TX is ready 
			; INX 			;incr index ptr
			; BRA ?NEXT		;loop to get next char 
; ?DONE:      RTS 

; ;Automatically pushes PBR, PCH, PCL and P on stack. Restored upon RTI. 
		; SEC					;set carry
		; XCE					;use it to go to 6502 native mode 
		; I8					;16bit X/Y 
		; A8					;8bit A 
		; ;show opening msg 
		; LDX #LDPRG			;start addr of msg (~$F809)
		; JSR PRINTMSG		;output msg (~$FF3D)
		; ;load Intel Hex format file 
; ;LOADINTEL: 
; ; 		:100130003F0156702B5E712B722B732146013421C7
; ; 		:00000001FF
; ;		: = SoL char / 10 = number of data bytes (0x10 = 16) 
; ; 		0130 = mem addr (0x0130) / 00 = record type (00=data;01=EoF)
; ;		3F 01 .. 34 21 = bytes of data / C7 = CRC
		; LDY #$00			;init buffer rd_ptr 
		; STY CRCCHECK		;start with reset CRCCHECK flag 
; ; INTELLINE
		; JSR GETCHAR			;get char from console 
		; STA IN,Y			;store it in input_buffer 
		; INY					;prep for next char
		; CMP #$1B			;was char = ESC? 
		; BNE NO_ESC			;no, continue processing  
		; BRL INTELDONE		;yes, abort 
; ; NO_ESC
		; CMP #$0D			;was char = CR? 
		; BNE INTELLINE		;nope, get next char 
		; LDY $FF				;CR encountered, so process one full line 
; ; FINDCOL
		; INY					;go back to start of buffer rd_ptr 
		; LDA IN,Y			;get 1st char
		; CMP #$3A			;was char = ':'? (=SoL) 
		; BNE FINDCOL			;nope, loop until ':'
		; INY					;':' found, skip it 
		; LDX #$00			;init running CRC
		; STX CRC				;store it 
		; JSR GETHEX			;read 2 ASCII chars and convert to HEX (=byte count)
		; STA COUNTER 		;expected number of bytes in HEX
		; CLC
		; ADC CRC				;update running CRC
		; STA CRC				;save it 
		; JSR GETHEX			;read 2 ASCII chars and convert to HEX (=mem addr high)
		; STA STH				;store mem addr high byte
		; CLC
		; ADC CRC				;update running CRC
		; STA CRC				;save it
		; JSR GETHEX			;read 2 ASCII chars and convert to HEX (=mem addr low)
		; STA STL				;store mem addr byte
		; CLC
		; ADC CRC				;update running CRC
		; STA CRC				;save it 
		; LDA #$2E			;print '.' for each full line read as progress indicator 
		; STA ACIA_DATA		;store in ACIA for it to output 
; ?SENT
		; LDA ACIA_STAT		;get ACIA status 
		; CMP #$10			;test for TX buffer empty
		; BEQ	?SENT			;loop ifnot yet sent 
; NODOT
		; JSR GETHEX			;read 2 ASCII chars and convert to HEX (=record type)			
		; CMP #$01			;was $01=EoF reached? 
		; BEQ INTELDONE		;yes, all lines read 
		; CLC					;more data 
		; ADC CRC				;update running CRCCRC				
		; STA CRC				;save it CRC
; INTELSTORE
		; JSR GETHEX			;read 2 ASCII chars and convert to HEX (=data)
		; STA (STL,X)			;store data at mem addr location 
		; CLC
		; ADC CRC				;update running CRC CRC
		; STA CRC				;save it  CRC
		; INC STL				;advance mem addr low location
		; BNE TESTCOUNT		; 
		; INC STH				;advance mem addr high location
; TESTCOUNT
		; DEC COUNTER			;update counter 
		; BNE INTELSTORE		;if counter not yet 0, continue processing 
		; JSR GETHEX			;read last 2 ASCII chars and convert to HEX (=CRC)
		; LDY #$00			;reset input buffer rd_ptr so we read from SoL again
		; CLC					;
		; ADC CRC				;add HEX file CRC to running CRC
		; BNE CRC_NOK			;if CRC is still 0, set CRC error flag 
		; BRL INTELLINE		;long branch to process next line 
; CRC_NOK
		; LDA #$01			;set a flag 
		; STA CRCCHECK		;to indicate a CRC error 
		; JMP INTELLINE		;process next line 
; INTELDONE 
		; LDA CRCCHECK		;get CRC error flag 
		; BEQ CRC_OKMESS		;when 0, all lines were ok, so show "ok" msg
		; LDX #CRCERR			;else, loading had a CRC error 
		; JSR PRINTMSG		;output an "error" msg saying so
		; ;show closing msg 
		; LDX #NEWLINE		;cursor to start of next line 
		; JSR PRINTMSG		
		; LDX #LDDONE			;reading of Intel HEX file is done (~$F832)
		; JSR PRINTMSG		;output msg (~$FF3D) 
		; RTI 				;all done (but with error), return from NMI_IRQ

; CRC_OKMESS
		; LDX #CRCOK			;CRC matched, so loading was ok 
		; JSR PRINTMSG		;output an "OK" msg
		; RTI					;all done (no error), return from NMI_IRQ 

; ;read 2 ASCII chars and converts them to 1 HEX byte
; ; input:  Y reg contains position of 1st ASCII char to read 
; ; output: A reg contains converted HEX value 
; GETHEX						
		; LDA IN,Y			;first char (0..9/A..F=0x30..39/0x41..46)
		; EOR #$30			;remove '0' ASCII offset (0..9/0x71..76)
		; CMP #$0A			;check if >9 (carry will be set for 9..76)
		; BCC DONEFIRST		;no, so char was 0..9 
		; ADC #$08			;yes, so char was A..H (0x7A..7F)
; DONEFIRST					
		; ASL					;move from lower to upper nibble (0..9/A..F)
		; ASL
		; ASL
		; ASL
		; STA L				;store upper nibble 
		; INY					
		; LDA IN,Y			;2nd char
		; EOR #$30			;remove '0' ASCII offset (0..9/0x71..76)
		; CMP #$0A            ;check if >9 (carry will be set for 9..76)
		; BCC DONESECOND      ;no, so char was 0..9 
		; ADC #$08            ;yes, so char was A..H (0x7A..7F)
; DONESECOND
		; AND #$0F			;keep lower nibble (0..9/A..F)
		; ORA L				;merge upper and lower nibble (00..FF)
		; INY					;move to next char
		; RTS 

; ;Subroutine that checks if ACIA has char in RX buffer 
; ;	Input: 	none
; ;	Output: char received 
; GETCHAR
		; LDA ACIA_STAT		;check if char received
		; AND #$08			;test bit3 (=ACIA_RX_FULL) 
		; BEQ GETCHAR			;loop until char received 
		; LDA ACIA_DATA		;return char in A reg 
		; RTS 

; *******E

; *******E
