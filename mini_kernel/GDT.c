#include "mini_kernel.h"
#include "GDT.h"

gdtdesc gdt[GDT_SIZE] = { 0 };
gdtr* _gdtrptr;

void init_gdt_desc(uint32_t Base, uint32_t Limit, uint8_t Access, uint8_t Other, gdtdesc* d) {
	d->lim0_15 = Limit & 0xFFFF;
	d->base0_15 = Base & 0xFFFF;
	d->base16_23 = (Base & 0xFF0000) >> 16;
	d->access = Access;
	d->lim16_19 = (Limit & 0xF0000) >> 16;
	d->other = Other & 0xF;
	d->base24_31 = (Base & 0xFF000000) >> 24;
}

void setup_gdt() {
	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &gdt[0]);

	// Kernel
	init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &gdt[1]);
	init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &gdt[2]);
	init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &gdt[3]);

	// User
	init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &gdt[4]);
	init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &gdt[5]);
	init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &gdt[6]);

	_gdtrptr = GDT_BASE + sizeof(gdt);

	//_gdtr.Limit = GDT_SIZE * 8;
	//_gdtr.Base = GDT_BASE;
	_gdtrptr->Limit = GDT_SIZE * 8;
	_gdtrptr->Base = GDT_BASE;

	_memcpy((uint8_t*)_gdtrptr->Base, (uint8_t*)gdt, _gdtrptr->Limit);
	_lgdt(_gdtrptr);

	reload_segments();
}