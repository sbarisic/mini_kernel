#include "mini_kernel.h"
#include "Entry.h"
#include "Interrupts.h"
#include "GDT.h"
#include "graphics.h"
#include "pci.h"

#define INIT_MSG(func, msg) do { com1_write_string("* " msg " initializing\n"); func; com1_write_string("* " msg " OK\n"); } while (0)

extern MULTIBOOT_HEADER multiboot_header;
MULTIBOOT_INFO* multiboot_info = 0;
char buffer[1024];

int console_command_count = 0;
console_command console_commands[1024] = { 0 };

void register_console_command(const char* name, console_func f) {
	console_commands[console_command_count].name = name;
	console_commands[console_command_count].f = f;
	console_command_count++;
}

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
	write(">");

	char* in = gets(buffer);
	write("\n");
	return in;
}

_declspec(noreturn) void kmain() {
	graphics_text_width = 80;
	graphics_text_height = 25;
	graphics_initialized = 0;
	console_enable(1);
	clear_screen();

	init_genrand(0);
	INIT_MSG(init_com1(), "COM1");
	INIT_MSG(setup_gdt(), "Global Descriptor Table");
	INIT_MSG(interrupts_init(), "Interrupts");

	// Calculate largest free address
	// Everything (kernel code, modules) shall be below that address
	MULTIBOOT_MOD* mods = multiboot_info->ModsAddr;
	uint32_t largest_free_address = 0;
	for (int i = 0; i < multiboot_info->ModsCount; i++) {
		uint32_t mod_end = mods[i].ModEnd + 128;

		if (mod_end > largest_free_address)
			largest_free_address = mod_end;
	}

	// Initialize memory manager that starts at largest free address and ends at the end of ram
	uint32_t mbytes_system = 0;
	INIT_MSG(kernel_allocator_parse_grub(multiboot_info->mmap_addr, multiboot_info->mmap_len, &mbytes_system, largest_free_address), "Memory allocator");

	// Init graphics
	graphics_init(multiboot_info->framebuffer_addr, multiboot_info->framebuffer_pitch, multiboot_info->framebuffer_width, multiboot_info->framebuffer_height,
				  multiboot_info->framebuffer_bpp);

	mods = multiboot_info->ModsAddr;
	for (int i = 0; i < multiboot_info->ModsCount; i++) {
		if (!_strcmp(mods[i].String, "bins/font.bin"))
			graphics_load_font(mods[i].ModStart);

		if (!_strcmp(mods[i].String, "bins/background.bin"))
			graphics_load_background(mods[i].ModStart);
	}

	clear_screen();
	int w, h;
	graphics_get_res(&w, &h);

	INIT_MSG(pci_init(), "PCI");
	INIT_MSG(register_default_console_commands(), "Default commands");
	INIT_MSG(register_turtle_console_commands(), "Turtle commands");

	stbsp_sprintf(buffer, "Resolution is %d x %d\n", w, h);
	write(buffer);

	stbsp_sprintf(buffer, "%d megabytes free\n", kernel_count_free_memory() / MEGABYTE);
	write(buffer);

	while (1) {
		char* input = readline();
		if (!*input)
			continue;

		char buffer2[1024];
		char* args[32] = { 0 };
		strcpy(buffer2, input);

		const char* delims = " \t";
		char* pch = strtok(buffer2, delims);
		int i = 0, len = 0, found_cmd = 0;

		while (pch != NULL) {
			/*memset(buffer, 0, sizeof(buffer));
			stbsp_sprintf(buffer, "%d - %s\n", i, pch);
			write(buffer);*/
			args[i] = pch;

			pch = strtok(NULL, delims);
			i++;
		}

		for (len = i, i = 0; i < sizeof(console_commands) / sizeof(*console_commands); i++) {
			if (console_commands[i].f == NULL)
				break;

			if (!_strcmp(args[0], console_commands[i].name)) {
				found_cmd = 1;
				console_commands[i].f(len, args, input);
				break;
			}
		}

		if (!found_cmd) {
			stbsp_sprintf(buffer, "%s: command not found\n", args[0]);
			write(buffer);
		}
	}
}