.include "zeropage.inc"

.export start
.exportzp disk_ptr
.exportzp disk_temp

.export _exit
.export __STARTUP__ : absolute = 1 ; CC65 will forceimport this

.import __STACKSIZE__
.import zero_initialize
.import _main

.segment "ZEROPAGE"
disk_ptr  = ptr1
disk_temp = ptr2

.segment "CODE"

start:
	; clear uninitialized RAM areas (except stack)
	jsr zero_initialize
	; clear stack
	ldx #0
	txa
	:
		sta $100, X
		inx
		bne :-
	; set stack position
	ldx #$FF
	txs
	; set C-stack position (lower half of hardware stack)
	lda #<($100 + __STACKSIZE__)
	ldx #>($100 + __STACKSIZE__)
	sta sp+0
	sta sp+1
	; NOTES vs standard cc65 crt0:
	;  zerobss - already taken care of by zero_initialize
	;  initlib/donelib - CONDES features not needed
	;  callmain - no command line, no arguments
	; call int main()
	jsr _main
_exit:
	pha
	txa
	pha
	; the original monitor RESET calls these 4 routines
	jsr $FE84 ; SETNORM
	jsr $FB2F ; INIT
	jsr $FE93 ; SETVID
	jsr $FE89 ; SETKBD
	; newline, then print exit code and enter monitor * prompt
	jsr $FD8E ; CROUT newline
	pla
	jsr $FDDA ; PRBYTE hex display
	pla
	jsr $FDDA
	jsr $FD8E
	jmp $FF69 ; MONZ monitor * prompt
