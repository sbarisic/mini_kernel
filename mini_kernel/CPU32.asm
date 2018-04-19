MB_PAGE_ALIGN	equ 1 << 0
MB_MEMINFO		equ 1 << 1
MB_VIDEO_MODE	equ 1 << 2
MB_KLUDGE		equ 1 << 16

FLAGS		equ MB_PAGE_ALIGN | MB_KLUDGE | MB_MEMINFO | MB_VIDEO_MODE
MAGIC		equ 0x1BADB002
CHECKSUM	equ -(MAGIC + FLAGS)

; INFO
global _multiboot_header

extern _kmain
extern _multiboot_info
extern _int_common

section .mboot
align 4
_multiboot_header:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

	dd _multiboot_header
	dd _multiboot_header
	dd 0
	dd 0
	dd entry

	; Mode type, width, height, depth
	dd 1
	dd 0
	dd 0
	dd 0

section .tnasm
entry:
	; Setup stack
	mov esp, stack.top

	; Setup pointers
	mov [_multiboot_info], ebx
	call _kmain

	cli
.hang:
	hlt
	jmp .hang



%macro	SAVE_REGS 0
	pushad
	push ds
	push es
	push fs
	push gs
	push ebx
	mov bx, 0x10
	mov ds, bx
	pop ebx
%endmacro

%macro	RESTORE_REGS 0
	pop gs
	pop fs
	pop es
	pop ds
	popad
%endmacro

%macro INTN 1
	global _int%1
	_int%1:
		cli

		SAVE_REGS
		push %1
		call _int_common
		pop eax
		RESTORE_REGS
		iret
%endmacro

%macro INTE 1
	global _int%1
	_int%1:
		cli

		SAVE_REGS
		push %1
		call _int_common
		pop eax
		RESTORE_REGS
		add esp, 4
		iret
%endmacro

INTN 100

INTN 	0	; #DE - Divide Error
INTN 	1
INTN 	2	; Non-Maskable Interrupt
INTN 	3	; #BP - Breakpoint
INTN 	4	; #OF - Overflow
INTN 	5	; #BR - Bound Range Exceeded
INTN	6	; #UD - Invalid Opcode
INTN 	7	; #NM - No Math Coprocessor
INTE	8	; #DF - Double Fault
INTN	9	; Coprocessor Segment Overrun
INTE	10	; #TS - Invalid TSS
INTE	11	; #NP - Segment Not Present
INTE	12	; #SS - Stack Segment Fault
INTE	13	; #GP - General Protection Fault
INTE	14	; #PF - Page Fault
INTN 	15
INTN 	16	; #MF - Math Fault
INTN 	17	; #AC - Alignment Check
INTN 	18	; #MC - Machine Check
INTN	19	; #XM - SIMD Exception

; IRQs
INTN 33
INTN 42

global _reload_segments
_reload_segments:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:_reload_segments.reload_cs
.reload_cs:
	ret

section .bss
align 4
stack:
	.bottom:
		resb 16384
	.top:
