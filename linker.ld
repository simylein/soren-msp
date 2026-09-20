MEMORY {
	fram (rx)  : ORIGIN = 0x4000, LENGTH = 0xc000
	ram  (rwx) : ORIGIN = 0x1c00, LENGTH = 0x2000
	vect (rx)  : ORIGIN = 0xff80, LENGTH = 0x80
}

SECTIONS {
	.text :{
		*(.text*)
		*(.rodata*)
	} > fram

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

	.reset 0xfffe :{
		KEEP(*(.reset))
	} > vect
}
