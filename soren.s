.section .text

.global reset
.global main

reset:
	mov.w	#0x3c00, r1
	call	#main

.section .reset

.word reset
