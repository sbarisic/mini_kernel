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


section .bss
align 4
stack:
	.bottom:
		resb 16384
	.top: