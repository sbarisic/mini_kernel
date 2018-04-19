#pragma once

typedef struct {
	uint16_t Limit;
	uint32_t Base;
} idtr;

typedef struct {
	uint16_t Offset0_15;
	uint16_t Select;
	uint16_t Type;
	uint16_t Offset16_31;
} idtdesc;

#define IDT_BASE 0x0
#define IDT_SIZE 0xFF

#define INTGATE  0x8E00
#define TRAPGATE 0xEF00	

void interrupts_init();

void interrupts_mask(uint8_t IRQ);
void interrupts_unmask(uint8_t IRQ);
void interrupts_reset(uint8_t IRQ);