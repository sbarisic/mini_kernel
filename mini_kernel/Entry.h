#pragma once

typedef struct {
	uint8_t Data;
} MULTIBOOT_HEADER;

typedef struct {
	uint32_t ModStart;
	uint32_t ModEnd;
	char* String;
	uint32_t Reserved;
} MULTIBOOT_MOD;

typedef struct {
	int32_t Size;
	uint64_t BaseAddr;
	uint64_t Len;
	uint32_t Type;
} MMAP_INFO;

/*
typedef struct {
	uint32_t Len;
	uint32_t Addr;
} MULTIBOOT_INFO_MMAP;*/

typedef struct {
	uint32_t Flags;

	uint32_t MemLower;
	uint32_t MemUpper;

	uint32_t BootDevice;

	uint32_t CmdLine;

	uint32_t ModsCount;
	uint32_t ModsAddr;

	uint64_t syms1;
	uint64_t syms2;

	uint32_t mmap_len;
	uint32_t mmap_addr;

	uint32_t drives_len;
	uint32_t drives_addr;

	uint32_t config_table;

	uint32_t boot_loader_name;

	uint32_t apm_table;

	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

	uint64_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t framebuffer_bpp;
} MULTIBOOT_INFO;

extern MULTIBOOT_INFO* multiboot_info;