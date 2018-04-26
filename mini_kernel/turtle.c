#include "mini_kernel.h"
#include "graphics.h"

static int screen_w;
static int screen_h;

static int turtle_x = 0;
static int turtle_y = 0;
static int turtle_pendown = 1;
static color turtle_color;

void turtle_putline(int x, int y, int x2, int y2) {
	if (turtle_pendown)
		graphics_putline(x + screen_w / 2, screen_h - y - screen_h / 2, x2 + screen_w / 2, screen_h - y2 - screen_h / 2, &turtle_color);
}

void t_setpos(int argc, const char** argv, const char* argi) {
	if (argc == 3) {
		turtle_x = _atoi(argv[1]);
		turtle_y = _atoi(argv[2]);
	}
}

void t_setclr(int argc, const char** argv, const char* argi) {
	if (argc == 4) {
		turtle_color.R = (uint8_t)(_atoi(argv[1]) % 256);
		turtle_color.G = (uint8_t)(_atoi(argv[2]) % 256);
		turtle_color.B = (uint8_t)(_atoi(argv[3]) % 256);
	}
}

void t_move(int argc, const char** argv, const char* argi) {
	if (argc == 3) {
		int dest_x = _atoi(argv[1]);
		int dest_y = _atoi(argv[2]);

		turtle_putline(turtle_x, turtle_y, turtle_x + dest_x, turtle_y + dest_y);

		turtle_x = turtle_x + dest_x;
		turtle_y = turtle_y + dest_y;
	}
}

void t_goto(int argc, const char** argv, const char* argi) {
	if (argc == 3) {
		int dest_x = _atoi(argv[1]);
		int dest_y = _atoi(argv[2]);

		turtle_putline(turtle_x, turtle_y, dest_x, dest_y);

		turtle_x = dest_x;
		turtle_y = dest_y;
	}
}

void t_penup(int argc, const char** argv, const char* argi) {
	turtle_pendown = 0;
}

void t_pendown(int argc, const char** argv, const char* argi) {
	turtle_pendown = 1;
}

void register_turtle_console_commands() {
	turtle_color = clr_white;
	graphics_get_res(&screen_w, &screen_h);

	register_console_command("t.setpos", &t_setpos);
	register_console_command("t.setclr", &t_setclr);
	register_console_command("t.move", &t_move);
	register_console_command("t.goto", &t_goto);
	register_console_command("t.penup", &t_penup);
	register_console_command("t.pendown", &t_pendown);
}