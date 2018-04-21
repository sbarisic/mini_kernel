#include "mini_kernel.h"

#define PORT 0x3F8

int com1_initialized = 0;

void init_com1() {
	__outbyte(PORT + 1, 0x00);    // Disable all interrupts
	__outbyte(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)

	__outbyte(PORT + 0, 0x0D);    // Set divisor to 12 (lo byte) 9600 baud
	__outbyte(PORT + 1, 0x00);    //                  (hi byte)

	__outbyte(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	__outbyte(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	__outbyte(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

	com1_initialized = 1;
}

uint8_t com1_read() {
	if (!com1_initialized)
		return 0;

	while (!(__inbyte(PORT + 5) & 1))
		;

	return __inbyte(PORT);
}

void com1_write(uint8_t byte) {
	if (!com1_initialized)
		return;

	while (!(__inbyte(PORT + 5) & 0x20))
		;

	__outbyte(PORT, byte);
}

void com1_write_string(const char* data) {
	if (!com1_initialized)
		return;

	while (*data)
		com1_write(*data++);
}

void com1_write_32(uint32_t num) {
	if (!com1_initialized)
		return;

	com1_write(num >> 24);
	com1_write(num >> 16);
	com1_write(num >> 8);
	com1_write(num);
}