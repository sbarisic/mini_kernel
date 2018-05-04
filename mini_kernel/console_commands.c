#include "mini_kernel.h"
#include "Entry.h"
#include "graphics.h"

extern console_command console_commands[];
static char buffer[1024] = { 0 };

void cmd_help(int argc, const char** argv, const char* argi) {
	int i = 0, j = 0;
	char name_buffer[16] = { 0 };

	while (console_commands[i].f != NULL) {
		memset(name_buffer, ' ', sizeof(name_buffer) - 1);
		name_buffer[sizeof(name_buffer) - 1] = 0;
		strcpy(name_buffer, console_commands[i].name);
		name_buffer[strlen(name_buffer)] = ' ';
		write(name_buffer);

		i++;
		if (++j >= 5) {
			j = 0;
			write("\n");
		}
	}

	write("\n");
}

void cmd_test(int argc, const char** argv, const char* argi) {
	for (int i = 0; i < argc; i++) {
		stbsp_sprintf(buffer, "%d - %s\n", i, argv[i]);
		write(buffer);
	}
}

void cmd_crash(int argc, const char** argv, const char* argi) {
	memset(0x800, 0, MEGABYTE);
	__asm mov esp, 1;
	__asm push eax;
}

void cmd_clear(int argc, const char** argv, const char* argi) {
	clear_screen();
}

void cmd_com(int argc, const char** argv, const char* argi) {
	if (argc >= 2) {
		if (!strcmp(argv[1], "hello")) {
			com1_write_string("Hello COM1 World!\n");
		} else if (!strcmp(argv[1], "init")) {
			int num = _atoi(argv[2]);

			char buffer[64];
			stbsp_sprintf(buffer, "Initializing serial on port 0x%X (%d)\n", num, num);
			write(buffer);

			init_com(num);
		} else {
			write(argv[1]);
			write(": com subcommand not found\n");
		}

		return;
	}

	write("com hello\n");
	write("com init [num]\n");
}

void cmd_echo(int argc, const char** argv, const char* argi) {
	if (argc > 1) {
		argi += strlen(argv[0]) + 1;
		write(argi);
		write("\n");
	}
}

void cmd_clr(int argc, const char** argv, const char* argi) {
	if (argc == 2 && !_strcmp(argv[1], "random")) {
		uint32_t clr0 = (uint32_t)genrand_int31();
		color clr = *(color*)&clr0;
		clr.A = 255;
		graphics_set_text_color(clr);
		return;
	}

	if (argc == 4) {
		uint8_t R = (uint8_t)(_atoi(argv[1]) % 256);
		uint8_t G = (uint8_t)(_atoi(argv[2]) % 256);
		uint8_t B = (uint8_t)(_atoi(argv[3]) % 256);
		graphics_set_text_color((color) {
			R, G, B, 255
		});
	} else
		graphics_set_text_color(clr_white);
}

void cmd_font(int argc, const char** argv, const char* argi) {
	if (argc == 2) {
		MULTIBOOT_MOD* mods = multiboot_info->ModsAddr;
		for (int i = 0; i < multiboot_info->ModsCount; i++) {
			if (!_strcmp(mods[i].String, argv[1])) {
				graphics_load_font(mods[i].ModStart);
				clear_screen();
				return;
			}
		}

		write(argv[1]);
		write(": font file not found\n");
	} else
		write("font [font_name]\n");
}

void register_default_console_commands() {
	register_console_command("cmd_test", &cmd_test);
	register_console_command("crash", &cmd_crash);
	register_console_command("clear", &cmd_clear);
	register_console_command("com", &cmd_com);
	register_console_command("echo", &cmd_echo);
	register_console_command("help", &cmd_help);
	register_console_command("clr", &cmd_clr);
	register_console_command("font", &cmd_font);
}