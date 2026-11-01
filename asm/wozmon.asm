WOZMON_ASM_INCLUDED	EQU 1

	INCLUDE ACIA.ASM					; using the ACIA for console input/output

INPUT_BUFFER 	EQU $1800               ; Circular input command buffer ($FF long) 
IN			 	EQU $1800               ; Circular input command buffer ($FF long) 

READ_PTR:       EQU $1901				; read index for the command buffer 
WRITE_PTR:      EQU $1902				; write index for the command buffer 

XAML            EQU $1924               ; Last "opened" location Low
XAMH            EQU $1925               ; Last "opened" location High
STL             EQU $1926               ; Store address Low
STH             EQU $1927               ; Store address High
L               EQU $1928               ; Hex value parsing Low
H               EQU $1929               ; Hex value parsing High
YSAV            EQU $192A               ; Used to see if hex value is given
MODE            EQU $192B               ; $00=XAM, $7F=STOR, $AE=BLOCK XAM

; ACIA_DATA 		EQU $7F80				; byte received or to be sent 
; ACIA_STATUS		EQU $7F81				; contains Tx/Rx status, error status, overrun/overflow, etc 
; ACIA_CMD 		EQU $7F82				; sets parity, interrupts used, etc 
; ACIA_CTRL 		EQU $7F83				; sets baudrate, stop/start/word length. etc

; PORTB			EQU	$7FE0				; I/O port B on VIA2 (LCD display) 
; PORTA			EQU	$7FE1				; I/O port A on VIA2 (LED bar graph) 
; DDRB			EQU	$7FE2				; data direction register B on VIA2 (1=out; 0=in)
; DDRA			EQU	$7FE3				; data direction register A on VIA2 (1=out; 0=in)

WOZMON: SECTION 
WOZ_RESET: 
	LDA 	READ_PTR		; init buffer by setting read and
	STA 	WRITE_PTR		; write index to same (random) value 

	; LDA     #$1F            ; 8-N-1, 19200 bps
	; STA     ACIA_CTRL
	LDY     #$8B            ; No parity, no echo, rx interrupts 
							; Bit7 MUST be set 
	; STY     ACIA_CMD

NOTCR: 
							; Y register now contains $8B
	CMP     #$08            ; Backspace key?
	BEQ     BACKSPACE       ; Yes.
	CMP     #$1B            ; ESC?
	BEQ     ESCAPE          ; Yes.
	INY                     ; Advance text index. After init, $8B becomes $8C 
	BPL     NEXTCHAR        ; Auto ESC if line longer than 127.

ESCAPE: 
	LDA     #$5C            ; "\".
	JSR     ECHO            ; Output it.

GETLINE: 
	LDA     #$0D            ; Send CR
	JSR     ECHO
	LDA     #$0A            ; Send LF
	JSR     ECHO

	LDY     #$01            ; Initialize text index.
BACKSPACE:      
	DEY                     ; Back up text index.
    BMI     GETLINE         ; If we go beyond SoL, write new prompt

NEXTCHAR: 
	JSR     CHRIN
	BCC     NEXTCHAR
	STA     IN,Y            ; Add to text buffer.
	CMP     #$0D            ; CR?
	BNE     NOTCR           ; No.

	LDY     #$FF            ; Reset text index.
	LDA     #$00            ; For XAM mode.
	TAX                     ; X=0.
SETBLOCK: 
	ASL
SETSTOR: 
	ASL                     ; Leaves $7B if setting STOR mode.
SETMODE: 
	STA     MODE            ; $00 = XAM, $74 = STOR, $B8 = BLOK XAM.
BLSKIP: 
	INY                     ; Advance text index.
NEXTITEM: 
	LDA     IN,Y            ; Get character.
	CMP     #$0D            ; CR?
	BEQ     GETLINE         ; Yes, done this line.
	CMP     #$2E            ; "."?
	BCC     BLSKIP          ; Skip delimiter.
	BEQ     SETBLOCK        ; Set BLOCK XAM mode.
	CMP     #$3A            ; ":"?
	BEQ     SETSTOR         ; Yes, set STOR mode.
	CMP     #$52            ; "R"?
	BEQ     RUNPROG         ; Yes, run user program.
	STX     L               ; $00 -> L.
	STX     H               ;    and H.
	STY     YSAV            ; Save Y for comparison

NEXTHEX: 
	LDA     IN,Y            ; Get character for hex test.
	EOR     #$30            ; Map digits to $0-9.
	CMP     #$0A            ; Digit?
	BCC     DIG             ; Yes.
	ADC     #$88            ; Map letter "A"-"F" to $FA-FF.
	CMP     #$FA            ; Hex letter?
	BCC     NOTHEX          ; No, character not hex.
DIG: 
	ASL
	ASL                     ; Hex digit to MSD of A.
	ASL
	ASL

	LDX     #$04            ; Shift count.
HEXSHIFT: 
	ASL                     ; Hex digit left, MSB to carry.
	ROL     L               ; Rotate into LSD.
	ROL     H               ; Rotate into MSD's.
	DEX                     ; Done 4 shifts?
	BNE     HEXSHIFT        ; No, loop.
	INY                     ; Advance text index.
	BNE     NEXTHEX         ; Always taken. Check next character for hex.

NOTHEX: 
	CPY     YSAV            ; Check if L, H empty (no hex digits).
	BEQ     ESCAPE          ; Yes, generate ESC sequence.

	BIT     MODE            ; Test MODE byte.
	BVC     NOTSTOR         ; B6=0 is STOR, 1 is XAM and BLOCK XAM.

	LDA     L               ; LSD's of hex data.
	STA     (STL,X)         ; Store current 'store index'.
	INC     STL             ; Increment store index.
	BNE     NEXTITEM        ; Get next item (no carry).
	INC     STH             ; Add carry to 'store index' high order.
TONEXTITEM:     
	JMP     NEXTITEM        ; Get next command item.

RUNPROG: 
	JMP     (XAML)          ; Run at current XAM index.

NOTSTOR: 
	BMI     XAMNEXT         ; B7 = 0 for XAM, 1 for BLOCK XAM.

	LDX     #$02            ; Byte count.
SETADR:         
	LDA     L-1,X           ; Copy hex data to
	STA     STL-1,X         ;  'store index'.
	STA     XAML-1,X        ; And to 'XAM index'.
	DEX                     ; Next of 2 bytes.
	BNE     SETADR          ; Loop unless X = 0.

NXTPRNT: 
	BNE     PRDATA          ; NE means no address to print.
	LDA     #$0D            ; CR.
	JSR     ECHO            ; Output it.
	LDA     #$0A            ; LF.
	JSR     ECHO            ; Output it.
	LDA     XAMH            ; 'Examine index' high-order byte.
	JSR     PRBYTE          ; Output it in hex format.
	LDA     XAML            ; Low-order 'examine index' byte.
	JSR     PRBYTE          ; Output it in hex format.
	LDA     #$3A            ; ":".
	JSR     ECHO            ; Output it.

PRDATA: 
	LDA     #$20            ; Blank.
	JSR     ECHO            ; Output it.
	LDA     (XAML,X)        ; Get data byte at 'examine index'.
	JSR     PRBYTE          ; Output it in hex format.
XAMNEXT:        
	STX     MODE            ; 0 -> MODE (XAM mode).
	LDA     XAML
	CMP     L               ; Compare 'examine index' to hex data.
	LDA     XAMH
	SBC     H
	BCS     TONEXTITEM      ; Not less, so no more data to output.

	INC     XAML
	BNE     MOD8CHK         ; Increment 'examine index'.
	INC     XAMH

MOD8CHK: 
	LDA     XAML            ; Check low-order 'examine index' byte
	AND     #$07            ; For MOD 8 = 0
	BPL     NXTPRNT         ; Always taken.

PRBYTE: 
	PHA                     ; Save A for LSD.
		LSR
		LSR
		LSR                     ; MSD to LSD position.
		LSR
		JSR     PRHEX           ; Output hex digit.
	PLA                     ; Restore A.

PRHEX: 
	AND     #$0F            ; Mask LSD for hex print.
	ORA     #$30            ; Add "0".
	CMP     #$3A            ; Digit?
	BCC     ECHO            ; Yes, output it.
	ADC     #$06            ; Add offset for letter.

ECHO: 
	STA     ACIA_DATA       ; Output character.
	PHA                     ; Save A.
		LDA     #$FF            ; Initialize delay loop.
TXDELAY:        
		DEC                     ; Decrement A.
		BNE     TXDELAY         ; Until A gets to 0.
	PLA                     ; Restore A.
	; LONGA ON
	; LONGI ON
	; REP 	#$20			; 16bit ACCU
	; REP		#$10 			; 16bit INDEX
	RTS                     ; Return.
	
CHRIN: 
	PHX
		JSR     BUFFER_SIZE
		BEQ     no_keypressed
		JSR     READ_BUFFER
		JSR     CHROUT                  ; echo
	PLX
	SEC
	RTS
no_keypressed: 
	PLX
	CLC
	RTS

CHROUT: 
	PHA
	STA     ACIA_DATA
	LDA     #$FF
txdelay:       	
	DEC
	BNE     txdelay
	PLA
	RTS

BUFFER_SIZE: 
	LDA WRITE_PTR
	SEC
	SBC READ_PTR
	RTS

; Write A Character (From The A Register) To The Circular Input Buffer
; Modifies: Flags, X
WRITE_BUFFER: 
	LDX WRITE_PTR
	STA INPUT_BUFFER,x
	INC WRITE_PTR
	RTS

; Read A Character From The Circular Input Buffer And Put It In The A Register
; Modifies: Flags, A, X
READ_BUFFER: 
	LDX READ_PTR
	LDA INPUT_BUFFER,x
	INC READ_PTR
	RTS
