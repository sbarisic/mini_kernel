#include "mini_kernel.h"
#include "graphics.h"

extern console_command console_commands[];
static char buffer[1024] = { 0 };

void cmd_help(int argc, const char** argv, const char* argi) {
	int i = 0, j = 0;
	char name_buffer[17] = { 0 };

	while (console_commands[i].f != NULL) {
		memset(name_buffer, ' ', sizeof(name_buffer) - 1);
		strcpy(name_buffer, console_commands[i].name);
		name_buffer[strlen(name_buffer)] = ' ';
		write(name_buffer);

		if (j > 2) {
			j = 0;
			write("\n");
		}

		i++;
		j++;
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
	__asm mov esp, 1;
	__asm push eax;
}

void cmd_clear(int argc, const char** argv, const char* argi) {
	clear_screen();
}

void cmd_com1(int argc, const char** argv, const char* argi) {
	if (argc == 2) {
		if (!strcmp(argv[1], "hello")) {
			com1_write_string("Hello COM1 World!\n");
		} else {
			write(argv[1]);
			write(": com1 subcommand not found\n");
		}

		return;
	}

	write("com1 hello\n");
}

void cmd_echo(int argc, const char** argv, const char* argi) {
	if (argc > 1) {
		argi += strlen(argv[0]) + 1;
		write(argi);
		write("\n");
	}
}

void cmd_clr(int argc, const char** argv, const char* argi) {
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

void register_default_console_commands() {
	register_console_command("cmd_test", &cmd_test);
	register_console_command("crash", &cmd_crash);
	register_console_command("clear", &cmd_clear);
	register_console_command("com1", &cmd_com1);
	register_console_command("echo", &cmd_echo);
	register_console_command("help", &cmd_help);
	register_console_command("clr", &cmd_clr);
}