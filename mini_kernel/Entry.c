#include "mini_kernel.h"
#include "Entry.h"
#include "Interrupts.h"
#include "GDT.h"
#include "graphics.h"

#define INIT_MSG(func, msg) do { write(msg " ... "); func(); write("OK\n"); } while (0)

extern MULTIBOOT_HEADER multiboot_header;
MULTIBOOT_INFO* multiboot_info = 0;
char buffer[1024];

void write_info(const char* str, int32_t n) {
	com1_write_string(str);
	write(str);

	char itoa_buf[64];
	_itoa(n, itoa_buf);
	com1_write_string(itoa_buf);
	write(itoa_buf);

	com1_write_string("\n");
	write("\n");
}

char* readline() {
	write("mini_kernel# ");

	char* in = gets(buffer);
	write("\n");
	return in;
}

_declspec(noreturn) void kmain() {
	graphics_text_width = 80;
	graphics_text_height = 25;
	graphics_initialized = 0;
	clear_screen();

	init_genrand(0);
	INIT_MSG(init_com1, "COM1");
	INIT_MSG(setup_gdt, "Global Descriptor Table");
	INIT_MSG(interrupts_init, "Interrupts");
	INIT_MSG(kernel_allocator_init, "Frame allocator");
	write("\n");

	graphics_init(multiboot_info->framebuffer_addr, multiboot_info->framebuffer_pitch,
				  multiboot_info->framebuffer_width, multiboot_info->framebuffer_height, multiboot_info->framebuffer_bpp);

	MULTIBOOT_MOD* mods = multiboot_info->ModsAddr;
	for (int i = 0; i < multiboot_info->ModsCount; i++) {
		if (!_strcmp(mods[i].String, "bins/font.bin"))
			graphics_load_font(mods[i].ModStart);

		if (!_strcmp(mods[i].String, "bins/background.bin"))
			graphics_load_background(mods[i].ModStart);
	}

	clear_screen();

	while (1) {
		char* input = readline();
		if (!*input)
			continue;

		if (!_strcmp(input, "com1_hello")) {
			com1_write_string("Hello COM1! How are you doing?\n");
		} else {
			write(input);
			write(": command not found\n");
		}
	}


	while (1)
		;
}