#include "mini_kernel.h"
#include "Entry.h"
#include "Interrupts.h"
#include "GDT.h"

extern MULTIBOOT_HEADER multiboot_header;
MULTIBOOT_INFO* multiboot_info;

_declspec(noreturn) void kmain() {
	setup_gdt();
	write("GDT ... OK\n");

	interrupts_init();
	write("Interrupts ... OK\n");



	while (1)
		;
}