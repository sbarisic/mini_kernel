#include "mini_kernel.h"

#define GRAPHICS_EXTERN
#include "graphics.h"

#define XY_TO_POS(x, y) (fb_pitch * (y) + bytesperpixel * (x))

static int font_w, font_h;
static color* font_data = NULL;

static int bg_w, bg_h;
static color* bg_data = NULL;

static uint8_t* fb;
static int fb_width, fb_height, fb_pitch, bytesperpixel;

void graphics_putpixel(int x, int y, color_fb clr) {
	if (x < 0 || x > fb_width)
		return;
	if (y < 0 || y > fb_height)
		return;

	*((color_fb*)&fb[XY_TO_POS(x, y)]) = clr;
}

void graphics_putpixel2(int32_t x, int32_t y, color* clr) {
	if (x < 0 || x > fb_width)
		return;
	if (y < 0 || y > fb_height)
		return;

	color* bg_clr = &bg_data[bg_w * y + x];
	color_fb fb_clr = { 0 };

	// TODO: proper alpha blending
	if (clr->A == 0) {
		fb_clr.R = bg_clr->R;
		fb_clr.G = bg_clr->G;
		fb_clr.B = bg_clr->B;
	} else {
		fb_clr.R = clr->R;
		fb_clr.G = clr->G;
		fb_clr.B = clr->B;
	}

	graphics_putpixel(x, y, fb_clr);
}

void graphics_clear(color clr) {
	for (int y = 0; y < fb_height; y++) {
		for (int x = 0; x < fb_width; x++) {
			graphics_putpixel2(x, y, &clr);
		}
	}
}

void graphics_blit_image(int xoffset, int yoffset, int w, int h, color* image_file) {
	graphics_blit_image2(xoffset, yoffset, 0, 0, w, h, w, h, image_file);
}

void graphics_blit_image2(int xoffset, int yoffset, int src_xoffset, int src_yoffset, int src_w, int src_h, int image_w, int image_h, color* image_data) {
	for (int y = 0; y < src_h; y++)
		for (int x = 0; x < src_w; x++)
			graphics_putpixel2(xoffset + x, yoffset + y, &image_data[image_w * (y + src_yoffset) + (x + src_xoffset)]);
}

void graphics_load_font(uint32_t* font_file) {
	font_w = font_file[0];
	font_h = font_file[1];
	font_data = font_file + 2;
}

void graphics_load_background(uint32_t* bg_file) {
	bg_w = bg_file[0];
	bg_h = bg_file[1];
	bg_data = bg_file + 2;
}

void graphics_blitchar(int32_t char_x, int32_t char_y, int32_t c) {
	if (c > 255)
		return;

	char_x *= 16;
	char_y *= 16;

	int32_t font_pos_x = (c % (font_w / font_char_w)) * font_char_w;
	int32_t font_pos_y = (c / (font_h / font_char_h)) * font_char_h;

	graphics_blit_image2(char_x, char_y, font_pos_x, font_pos_y, font_char_w, font_char_h, font_w, font_h, font_data);
}

void graphics_init(uint8_t* fbaddr, uint32_t pitch, uint32_t width, uint32_t height, uint32_t bpp) {
	fb = fbaddr;
	fb_width = width;
	fb_height = height;
	fb_pitch = pitch;
	bytesperpixel = bpp / 8;

	SET_COLOR(clr_transparent, 0, 0, 0, 0);
	SET_COLOR(clr_red, 255, 0, 0, 255);
	SET_COLOR(clr_green, 0, 255, 0, 255);
	SET_COLOR(clr_blue, 0, 0, 255, 255);
	SET_COLOR(clr_white, 255, 255, 255, 255);

	font_char_w = font_char_h = 16;
	graphics_text_width = width / font_char_w;
	graphics_text_height = height / font_char_h;
	graphics_initialized = 1;
}