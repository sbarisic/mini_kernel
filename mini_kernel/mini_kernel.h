#pragma once

#include <intrin.h>
#include <intrin0.h>

#include <string.h>
#include <stdint.h>

typedef void(*syscall_func)(void* data);

// System calls
void register_syscall(int n, syscall_func func);
void syscall(int a, void* b);

// Serial
void init_com1();
uint8_t com1_read();
void com1_write(uint8_t byte);
void com1_write_string(const char* data);
void com1_write_32(uint32_t num);

// Console input/output
void write(const char* Msg);
char* gets(char* str);
void clear_screen();
void conin_putchar(uint8_t c);
void event_scancode(uint32_t scode);

// Memory
void _memcpy(uint8_t* dest, uint8_t* source, int len);
void _memset(uint8_t* dest, uint8_t val, int len);
int _memshift(uint8_t* buffer, uint32_t buffer_len, uint32_t count);
int _strcmp(const char* a, const char* b);
void _itoa(int n, char s[]);