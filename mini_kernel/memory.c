#include "mini_kernel.h"

void _memcpy(uint8_t* dest, uint8_t* source, int len) {
	for (int i = 0; i < len; i++)
		dest[i] = source[i];
}

void _memset(uint8_t* dest, uint8_t val, int len) {
	for (int i = 0; i < len; i++)
		dest[i] = val;
}