#include "Entry.h"

extern MULTIBOOT_HEADER multiboot_header;
MULTIBOOT_INFO* multiboot_info;

struct {
	int X, Y;
	uint16_t* Memory;
} ConOut;

void __cdecl write(const char* Msg) {
	while (*Msg) {
		ConOut.Memory[ConOut.Y * 80 + ConOut.X] = (uint16_t)(*Msg) | 15 << 8;

		Msg++;
		ConOut.X++;
		if (ConOut.X >= 80) {
			ConOut.Y++;
			ConOut.X = 0;
		}
	}
}

_declspec(noreturn) void kmain() {
	ConOut.X = 0;
	ConOut.Y = 0;
	ConOut.Memory = (uint16_t*)0xb8000;

	write("Hello Kernel World!");

	while (1)
		;
}