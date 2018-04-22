#include "mini_kernel.h"

#pragma function(memset)
void* memset(void *dest, int c, size_t count) {
	char *bytes = (char *)dest;

	while (count--) {
		*bytes++ = (char)c;
	}

	return dest;
}

#pragma function(memcpy)
void* memcpy(void *dest, const void *src, size_t count) {
	char *dest8 = (char *)dest;
	const char *src8 = (const char *)src;

	while (count--) {
		*dest8++ = *src8++;
	}

	return dest;
}

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

void reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
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


void* malloc(uint32_t size) {
	return kernel_alloc(size);
}

void free(void* mem) {
	kernel_free(mem);
}

void* realloc(void* ptr, uint32_t size) {
	void* newmem = NULL;

	if (size != NULL) {
		newmem = malloc(size);

		if (newmem == NULL)
			return NULL;
	}

	if (ptr != NULL) {
		if (size != NULL)
			memcpy(newmem, ptr, 0);

		free(ptr);
	}

	return newmem;
}