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

char * strstr(const char * s1, const char * s2) {
	const char * p1 = s1;
	const char * p2;
	while (*s1)
	{
		p2 = s2;
		while (*p2 && (*p1 == *p2))
		{
			++p1;
			++p2;
		}
		if (!*p2)
		{
			return (char *)s1;
		}
		++s1;
		p1 = s1;
	}
	return NULL;
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

void reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

char* strtok(char* s1, const char* s2) {
	static char * tmp = NULL;
	const char * p = s2;

	if (s1 != NULL)
		tmp = s1;
	else {
		if (tmp == NULL)
			return NULL;
		s1 = tmp;
	}

	while (*p && *s1) {
		if (*s1 == *p) {
			++s1;
			p = s2;
			continue;
		}
		++p;
	}

	if (!*s1)
		return (tmp = NULL);

	tmp = s1;
	while (*tmp) {
		p = s2;
		while (*p) {
			if (*tmp == *p++) {
				*tmp++ = '\0';
				return s1;
			}
		}
		++tmp;
	}

	tmp = NULL;
	return s1;
}

int isnumeric(char x) {
	return (x >= '0' && x <= '9') ? 1 : 0;
}

int _atoi(char *str) {
	if (*str == NULL)
		return 0;

	int res = 0;
	int sign = 1;
	int i = 0;

	if (str[0] == '-') {
		sign = -1;
		i++;
	}

	for (; str[i] != '\0'; ++i) {
		if (!isnumeric(str[i]))
			return 0;
		res = res * 10 + str[i] - '0';
	}

	return sign * res;
}