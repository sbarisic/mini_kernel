#include "mini_kernel.h"
#include "Interrupts.h"
#include "keyboard.h"

uint8_t IRQ_mask = 0;
idtdesc idt[IDT_SIZE] = { 0 };
idtr _idtr;

void init_idt_desc(uint16_t Select, void* Offset, uint16_t Type, idtdesc* d) {
	d->Offset0_15 = ((uint32_t)Offset) & 0xFFFF;
	d->Select = Select;
	d->Type = Type;
	d->Offset16_31 = (((uint32_t)Offset) & 0xFFFF0000) >> 16;
}

void interrupts_mask(uint8_t IRQ) {
	IRQ_mask |= (1 << IRQ);

	if (IRQ > 7)
		__outbyte(0xA1, IRQ_mask >> 8);
	else
		__outbyte(0x21, IRQ_mask & 0xFF);
}

void interrupts_unmask(uint8_t IRQ) {
	IRQ_mask &= ~(1 << IRQ);

	if (IRQ > 7)
		__outbyte(0xA1, IRQ_mask >> 8);
	else
		__outbyte(0x21, IRQ_mask & 0xFF);
}

void interrupts_reset(uint8_t IRQ) {
	if (IRQ > 8)
		__outbyte(0xA0, 0x20);

	__outbyte(0x20, 0x20);
}

#define __int(n) extern void int ## n ()
__int(13);
__int(14);
__int(33);
__int(100);
#undef __int
#define __int(n) int ## n

extern void int_syscall();

void interrupts_init() {
	for (int i = 0; i < IDT_SIZE; i++)
		init_idt_desc(0x08, __int(100), INTGATE, &idt[i]);

#define HOOK_INT(n) init_idt_desc(0x08, __int(n), INTGATE, &idt[n])
	HOOK_INT(13); // GP
	HOOK_INT(14); // PF
	HOOK_INT(33); // Keyboard
#undef HOOK_INT

	init_idt_desc(0x08, int_syscall, INTGATE, &idt[42]);


	_idtr.Limit = IDT_SIZE * 8;
	_idtr.Base = IDT_BASE;

	_memcpy((uint8_t*)_idtr.Base, (uint8_t*)idt, _idtr.Limit);
	__lidt(&_idtr);

	// Init PIC
	__outbyte(0x20, 0x11); // Init master, slave
	__outbyte(0xA0, 0x11);

	__outbyte(0x21, 0x20); // Master mapped to 0x20 - 0x27
	__outbyte(0xA1, 0x28); // Slave mapped to 0x28 - 0x2E

	__outbyte(0x21, 0x04);
	__outbyte(0xA1, 0x02);

	__outbyte(0x21, 0x01);
	__outbyte(0xA1, 0x01);

	// Master, slave mask
	__outbyte(0x21, 0x00);
	__outbyte(0xA1, 0x00);

	__asm sti;
}

void int_common(int ID) {
	switch (ID) {
		case 13:
			write("GENERAL PROTECTION FAULT\n");
			while (1)
				;
			break;

		case 14:
			write("PAGE FAULT\n");
			while (1)
				;
			break;

		case 33:
			keyboard_handle_interrupt();
			break;

		case 42:
			write("HAI!\n");
			break;

		default:
			break;
	}

	__outbyte(0x20, 0x20);
	__outbyte(0xA0, 0x20);
}