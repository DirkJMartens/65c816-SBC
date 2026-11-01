;:ts=8
; General purpose ROM startup code for 65C02
;
; First we set up the stack pointer.
; Then we copy the initialized data.
; Then we clear out the uninitialized data.
; Then we call main.

	chip	65C02

	include	"zpage.inc"

	****************************************************************
	*                                                              *
	*   Define a special section in case not very much code.       *
	*                                                              *
	****************************************************************

MCH_STACK	EQU	$FF 		; machine stack
C_STACK		EQU	$D7FF		; C pseudo stack

STARTUP	SECTION	OFFSET $FF00

	xref  _BEG_DATA
	xref  _END_DATA
	xref  _ROM_BEG_DATA
	xref  _BEG_UDATA
	xref  _END_UDATA

START:
	cld			; clear decimal mode
	ldx	#MCH_STACK	; get the hardware stack address
	txs			; and set the stack to it
	lda	#<C_STACK	; get the software stack address
	sta	SP
	lda	#>C_STACK
	sta	SP+1

	****************************************************************
	*                                                              *
	*    Next, we want to copy the initialized data from           *
	*      ROM to RAM.                                             *
	*                                                              *
	****************************************************************

	lda	#<_END_DATA-_BEG_DATA	;number of bytes to copy
	sta	R0
	lda	#>_END_DATA-_BEG_DATA
	sta	R0+1
	ora	R0
	beq	copydone		; nothing to do
	lda	#<_ROM_BEG_DATA		; get source into R1
	sta	R1
	lda	#>_ROM_BEG_DATA
	sta	R1+1
	lda	#<_BEG_DATA		; get dest into R2
	sta	R2
	lda	#>_BEG_DATA
	sta	R2+1
	ldy	#0
copyloop:
	lda	(R1),Y
	sta	(R2),Y
	iny
	bne	copyskip
	inc	R1+1
	inc	R2+1
copyskip:
	lda	R0
	bne	copyskip2
	dec	R0+1
	bmi	copydone
copyskip2:
	dec	R0
	bra	copyloop
copydone:

	****************************************************************
	*                                                              *
	*   Now, clear out the uninitialized data area.                *
	*    We assume that it is in the same bank as DATA.            *
	*                                                              *
	****************************************************************

	lda	#<_END_UDATA-_BEG_UDATA  ;get number of bytes to clear
	sta	R0
	lda	#>_END_UDATA-_BEG_UDATA
	sta	R0+1
	ora	R0
	beq	done
	lda	#<_BEG_UDATA		; get beginning of zeroes
	sta	R1
	lda	#>_BEG_UDATA
	sta	R1+1
	lda	#0			;get a zero for storing
	tay
loop:
	tya
	sta	(R1),y			; clear memory
	inc	R1
	bne	skip
	inc	R1+1
skip:
	lda	R0
	bne	skip2
	dec	R0+1
	bmi	done
skip2:
	dec	R0
	bra	loop			;continue till done
done:

	****************************************************************
	*                                                              *
	*   Finally, transfer control to the real program.             *
	*                                                              *
	****************************************************************

        xref	_main
        jsr	_main
	brk

        data
        xdef  _heap_start,_heap_end
_heap_start:
        word  $D800
_heap_end:
        word  $ff00
        ends

	****************************************************************
	*                                                              *
	*   This section defines the interrupt and reset vectors.      *
	*      The reset vectors always start at FFFA. Replace the     *
	*      zeroes with any routines that handle interrupts.        *
	*                                                              *
	****************************************************************

	org   $FFFA

V_NMI	dw	0
V_RESET	dw	START
V_IRQ	dw	0

	ends
	end

