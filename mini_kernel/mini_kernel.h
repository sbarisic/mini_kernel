#pragma once

#include <intrin.h>
#include <intrin0.h>

#include <string.h>
#include <stdint.h>

void write(const char* Msg);
void keyboard_putchar(char c);

void _memcpy(uint8_t* dest, uint8_t* source, int len);
void _memset(uint8_t* dest, uint8_t val, int len);