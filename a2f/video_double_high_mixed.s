; video_double_high_mixed
;
; Common routines for double high resolution mixed modes

.include "../a2f.inc"

.export video_page_double_high_mixed
.export video_page_copy_double_high_mixed
.export video_cls_double_high_mixed

.import video_page_apply
.import video_cls_page

.proc video_page_double_high_mixed
	; set mode
	sta $C050 ; graphics mode (TEXT)
	sta $C053 ; mixed (MIXED)
	sta $C057 ; high-res (HIRES)
	; disable double mode
	sta $C00D ; 80 columns (80COL)
	sta $C07E ; enable DHIRES switch (IOUDIS)
	sta $C05E ; double-hires on (AN3/DHIRES)
	jmp video_page_apply
.endproc

.proc video_page_copy_double_high_mixed
	; TODO
	rts
.endproc

.proc video_cls_double_high_mixed
	; reset cursor
	lda video_text_xr
	sta video_text_x
	lda video_text_yr
	sta video_text_y
	; clear graphics
	lda video_page_w
	and #1
	eor #CLS_DHIGH0
	tax
	lda #0
	jsr video_cls_page
	; clear mixed text
	lda video_page_w
	and #1
	eor #CLS_DMIXED0
	tax
	lda #$A0 ; space, normal
	jmp video_cls_page
.endproc
