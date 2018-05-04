#pragma once

#include <intrin.h>
#include <intrin0.h>

#include <string.h>
#include <stdint.h>

#include "random.h"
#include "stb_sprintf.h"

#define STBI_ASSERT
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_ONLY_PNG
#include "stb_image.h"

#define TRUE 1
#define FALSE 0

#define MEGABYTE 0x100000

typedef void(*syscall_func)(void* data);
typedef void(*console_func)(int argc, const char** argv, const char* argi);

typedef struct {
	const char* name;
	console_func f;
} console_command;

void register_default_console_commands();
void register_turtle_console_commands();
void register_console_command(const char* name, console_func f);

// System calls
void register_syscall(int n, syscall_func func);
void syscall(int a, void* b);

// Serial
void init_com1();
uint8_t com1_read();
void com1_write(uint8_t byte);
void com1_write_string(const char* data);
void com1_write_32(uint32_t num);

void init_com(uint16_t PORT_N);
void com_get_info(uint16_t* cur_port);

// Console input/output
void console_enable(int enable);
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
void reverse(char s[]);
int isnumeric(char x);
int _atoi(char *str);

void kernel_allocator_init(uint32_t free_memory, uint32_t free_memory_len);
void* kernel_alloc(uint32_t size);
void kernel_free(void* frame);
void* kernel_alloc_copy(void* memory, uint32_t len);
void* kernel_alloc_in_place(void* mem, uint32_t size);
//void* kernel_realloc_in_place(void* mem, uint32_t size);
uint32_t kernel_count_free_memory();
uint32_t kernel_allocator_parse_grub(uint32_t mmap_addr, uint32_t mmap_len, uint32_t* system_mbytes, uint32_t discard_below);