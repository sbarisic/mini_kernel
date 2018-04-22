#pragma once

typedef struct {
	uint8_t B;
	uint8_t G;
	uint8_t R;
	uint8_t Padding;
} color_fb;

void graphics_init(uint8_t* fbaddr, uint32_t pitch, uint32_t width, uint32_t height, uint32_t bpp);