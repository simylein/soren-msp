.equ ta0ccr1, 0x0354

.section .text

.global reset
.global main
.global timer_isr

reset:
	mov.w #0x5a80, &0x015c
	mov.w	#0x3c00, r1
	call	#main

timer_isr:
	dec.w	&ta0ccr1
	jnz	timer_done

	bic.w	#0x00d0, 0(sp)

timer_done:
	reti


.section .timer_a0, "a", @progbits
.word timer_isr

.section .reset, "a", @progbits
.word reset
