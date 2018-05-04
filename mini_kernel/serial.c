#include "mini_kernel.h"
#include "pci.h"

#define COM1 0x3F8

static uint16_t PORT;
int com_initialized = 0;

void init_com(uint16_t PORT_N) {
	PORT = PORT_N;

	__outbyte(PORT + 1, 0x00);    // Disable all interrupts
	__outbyte(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)

	__outbyte(PORT + 0, 0x0D);    // Set divisor to 12 (lo byte) 9600 baud
	__outbyte(PORT + 1, 0x00);    //                  (hi byte)

	__outbyte(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	__outbyte(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	__outbyte(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

	com_initialized = 1;
}

uint8_t com_read(uint16_t PORT) {
	if (!com_initialized)
		return 0;

	while (!(__inbyte(PORT + 5) & 1))
		;

	return __inbyte(PORT);
}

void com_write(uint16_t PORT, uint8_t byte) {
	if (!com_initialized)
		return;

	while (!(__inbyte(PORT + 5) & 0x20))
		;

	__outbyte(PORT, byte);
}

void com_write_string(uint16_t PORT, const char* data) {
	if (!com_initialized)
		return;

	while (*data)
		com_write(PORT, *data++);
}

void com_write_32(uint16_t PORT, uint32_t num) {
	if (!com_initialized)
		return;

	com_write(PORT, num >> 24);
	com_write(PORT, num >> 16);
	com_write(PORT, num >> 8);
	com_write(PORT, num);
}

void com_get_info(uint16_t* cur_port) {
	if (cur_port)
		*cur_port = PORT;
}

// COM1 stuff

void init_com1() {
	init_com(COM1); // Init default COM port
}

uint8_t com1_read() {
	return com_read(PORT);
}

void com1_write(uint8_t byte) {
	com_write(PORT, byte);
}

void com1_write_string(const char* data) {
	com_write_string(PORT, data);
}

void com1_write_32(uint32_t num) {
	com_write_32(PORT, num);
}