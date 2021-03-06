#include "mini_kernel.h"

void _memcpy(uint8_t* dest, uint8_t* source, int len) {
	memcpy(dest, source, len);
}

void _memset(uint8_t* dest, uint8_t val, int len) {
	memset(dest, val, len);
}

int _memshift(uint8_t* buffer, uint32_t buffer_len, uint32_t count) {
	if (count < 1 || count >= buffer_len)
		return 1;

	for (int i = 0; i < buffer_len - count; i++)
		buffer[i] = buffer[i + count];

	for (int i = buffer_len - count; i < buffer_len; i++)
		buffer[i] = 0;

	return 0;
}

int _strcmp(const char* a, const char* b) {
	while (*a && (*a == *b)) {
		a++;
		b++;
	}

	return *(const unsigned char*)a - *(const unsigned char*)b;
}

void _itoa(int n, char s[]) {
	int i, sign;

	if ((sign = n) < 0)
		n = -n;

	i = 0;

	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0)
		s[i++] = '-';

	s[i] = '\0';
	reverse(s);
}