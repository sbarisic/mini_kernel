#include "mini_kernel.h"

#define CONOUT_WIDTH 80

struct {
	int X, Y;
	uint16_t* Memory;
} ConOut = { 0, 0, 0xb8000 };

void write(const char* Msg) {
	while (*Msg) {
		char C = *Msg;

		if (C == '\n') {
			ConOut.X = 0;
			ConOut.Y++;
		} else {
			ConOut.Memory[ConOut.Y * CONOUT_WIDTH + ConOut.X] = (uint16_t)(*Msg) | 15 << 8;
			ConOut.X++;
		}

		Msg++;
		if (ConOut.X >= 80) {
			ConOut.Y++;
			ConOut.X = 0;
		}
	}
}

void keyboard_putchar(char c) {
	char str[2] = { c, 0 };
	write(str);
}