#include "mini_kernel.h"
#include "graphics.h"

#define XY_TO_POS(x, y) (fb_pitch * (y) + bytesperpixel * (x))

static uint8_t* fb;
static int fb_width, fb_height, fb_pitch, bytesperpixel;

void graphics_putpixel(int x, int y, color_fb clr) {
	if (x < 0 || x > fb_width)
		return;
	if (y < 0 || y > fb_height)
		return;

	*((color_fb*)&fb[XY_TO_POS(x, y)]) = clr;
}

void graphics_clear(color_fb clr) {
	for (int y = 0; y < fb_height; y++) {
		for (int x = 0; x < fb_width; x++) {
			graphics_putpixel(x, y, clr);
		}
	}
}

void graphics_clear_rgb(uint8_t R, uint8_t G, uint8_t B) {
	graphics_clear((color_fb) {
		B, G, R, 0
	});
}

void graphics_init(uint8_t* fbaddr, uint32_t pitch, uint32_t width, uint32_t height, uint32_t bpp) {
	fb = fbaddr;
	fb_width = width;
	fb_height = height;
	fb_pitch = pitch;
	bytesperpixel = bpp / 8;

	color_fb clr;
	clr.Padding = 0;

	for (int y = 0; y < fb_height; y++) {
		for (int x = 0; x < fb_width; x++) {
			if (x % 10 == 0 || y % 10 == 0) {
				clr.R = (uint8_t)(genrand_int31() % 255);
				clr.G = (uint8_t)(genrand_int31() % 255);
				clr.B = (uint8_t)(genrand_int31() % 255);
			}

			graphics_putpixel(x, y, clr);
		}
	}
}