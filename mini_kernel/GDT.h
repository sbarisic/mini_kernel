#pragma once

typedef struct {
	uint16_t Limit;
	uint32_t Base;
} gdtr;

typedef struct {
	uint16_t lim0_15;
	uint16_t base0_15;
	uint8_t base16_23;
	uint8_t access;
	uint8_t lim16_19 : 4;
	uint8_t other : 4;
	uint8_t base24_31;
} gdtdesc;

#define GDT_BASE 0x800
#define GDT_SIZE 0xFF

void setup_gdt();