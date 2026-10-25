MEMORY {
	firmware (rx) : ORIGIN = 0x4000, LENGTH = 0x4000
	ram (rwx) : ORIGIN = 0x1c00, LENGTH = 0x2000
	fram (rw) : ORIGIN = 0x8000, LENGTH = 0x7f00
	config (rw) : ORIGIN = 0xff00, LENGTH = 0x80
	vect (rx) : ORIGIN = 0xff80, LENGTH = 0x80
}

SECTIONS {
	.text :{
		*(.text*)
		*(.rodata*)
	} > firmware

	.data :{
		*(.data*)
	} > ram

	.bss :{
		*(.bss*)
		*(common)
	} > ram

	.vectors :{
		. = ALIGN(2);
		KEEP(*(.vectors))
	} > vect

	.timer_a0 0xffea :{
		KEEP(*(.timer_a0))
	} > vect

	.reset 0xfffe :{
		KEEP(*(.reset))
	} > vect
}
