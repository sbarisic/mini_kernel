#include "mini_kernel.h"

#define CONOUT_WIDTH 80
#define CONOUT_HEIGHT 25

#define CONIN_LEN 4096

struct {
	int ReadingInput;
	int Idx;
	char Buffer[CONIN_LEN + 1];
} ConIn = { 0 };

struct {
	int X, Y;
	uint16_t* Memory;
} ConOut = { 0, 0, 0xb8000 };

void clear_screen() {
	ConOut.X = ConOut.Y = 0;
	_memset(ConOut.Memory, 0, CONOUT_WIDTH * CONOUT_HEIGHT * 2);
}

void write(const char* Msg) {
	com1_write_string(Msg);

	while (*Msg) {
		char C = *Msg;

		if (C == '\n') {
			ConOut.X = 0;
			ConOut.Y++;
		} else if (C == '\r') {
			ConOut.X = 0;
		} else if (C == '\b') {
			if (ConOut.X > 0) {
				ConOut.X--;
				write(" ");
				ConOut.X--;
				return;
			}
		} else {
			ConOut.Memory[ConOut.Y * CONOUT_WIDTH + ConOut.X] = (uint16_t)(*Msg) | 15 << 8;
			ConOut.X++;
		}

		Msg++;
		if (ConOut.X >= CONOUT_WIDTH) {
			ConOut.Y++;
			ConOut.X = 0;
		}

		if (ConOut.Y >= CONOUT_HEIGHT) {
			ConOut.Y--;
			_memshift(ConOut.Memory, CONOUT_WIDTH * CONOUT_HEIGHT * 2, CONOUT_WIDTH * 2);
		}
	}
}

char* gets(char* str) {
	ConIn.ReadingInput = 1;
	ConIn.Idx = 0;

	while (ConIn.ReadingInput)
		;

	_memset(str, 0, ConIn.Idx + 1);
	_memcpy(str, ConIn.Buffer, ConIn.Idx);
	ConIn.Idx = 0;

	return str;
}

int input_shiftl(int Len) {
	if (_memshift(ConIn.Buffer, CONIN_LEN, Len))
		return 1;

	ConIn.Idx -= Len;
	return 0;
}

int input_move(char* dest, int Len) {
	int Min = Len > ConIn.Idx ? ConIn.Idx : Len;

	for (int i = 0; i < Min; i++)
		dest[ConIn.Buffer[i]];

	input_shiftl(Min);
}

void input_clear() {
	ConIn.Idx = 0;
}

void conin_putchar(uint8_t c) {
	if (!ConIn.ReadingInput)
		return;

	if (ConIn.Idx >= CONIN_LEN)
		if (input_shiftl(5))
			return;

	if (c == '\n') {
		ConIn.ReadingInput = 0;
		return;
	} else if (c == '\b') {
		if (ConIn.Idx > 0) {
			ConIn.Idx--;
			write("\b");
		}

		return;
	}

	char tmp[2] = { c, 0 };
	write(tmp);
	ConIn.Buffer[ConIn.Idx++] = c;
}

void event_scancode(uint32_t scode) {

}