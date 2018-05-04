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

static color multiply_color = {
	255, 255, 255, 255
};

static color text_color = {
	255, 255, 255, 255
};

void graphics_get_res(int* w, int* h) {
	*w = fb_width;
	*h = fb_height;
}

void graphics_set_blend_color(color clr) {
	multiply_color = clr;
}

color graphics_get_blend_color() {
	return multiply_color;
}

void graphics_set_text_color(color clr) {
	text_color = clr;
}

color graphics_get_text_color() {
	return text_color;
}

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

		fb_clr.R = (uint8_t)((fb_clr.R * (int32_t)multiply_color.R) / 255);
		fb_clr.G = (uint8_t)((fb_clr.G * (int32_t)multiply_color.G) / 255);
		fb_clr.B = (uint8_t)((fb_clr.B * (int32_t)multiply_color.B) / 255);
	}

	graphics_putpixel(x, y, fb_clr);
}

void graphics_putline(int x0, int y0, int x1, int y1, color* clr) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	for (;;) {
		graphics_putpixel2(x0, y0, clr);

		if (x0 == x1 && y0 == y1)
			break;

		e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void graphics_putcircle(int xm, int ym, int r, color* clr) {
	int x = -r, y = 0, err = 2 - 2 * r;

	do {
		graphics_putpixel2(xm - x, ym + y, clr); // 1st quadrant
		graphics_putpixel2(xm - y, ym - x, clr); // 2nd quadrant
		graphics_putpixel2(xm + x, ym - y, clr); // 3rd quadrant
		graphics_putpixel2(xm + y, ym + x, clr); // 4th quadrant
		r = err;

		if (r > x)
			err += ++x * 2 + 1;

		if (r <= y)
			err += ++y * 2 + 1;
	} while (x < 0);
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

	const int char_count_x = 16;
	const int char_count_y = 16;

	//font_char_w = 16;
	//font_char_h = 16;
	font_char_w = font_w / char_count_x;
	font_char_h = font_h / char_count_y;

	graphics_text_width = fb_width / font_char_w;
	graphics_text_height = fb_height / font_char_h;
}

void graphics_load_background(uint32_t* bg_file) {
	bg_w = bg_file[0];
	bg_h = bg_file[1];
	bg_data = bg_file + 2;
}

void graphics_blitchar(int32_t char_x, int32_t char_y, int32_t c) {
	if (c > 255)
		return;

	char_x *= font_char_w;
	char_y *= font_char_h;

	int32_t font_pos_x = (c % (font_w / font_char_w)) * font_char_w;
	int32_t font_pos_y = (c / (font_h / font_char_h)) * font_char_h;

	color old_blend = graphics_get_blend_color();
	graphics_set_blend_color(graphics_get_text_color());
	graphics_blit_image2(char_x, char_y, font_pos_x, font_pos_y, font_char_w, font_char_h, font_w, font_h, font_data);
	graphics_set_blend_color(old_blend);
}

void graphics_init(uint8_t* fbaddr, uint32_t pitch, uint32_t width, uint32_t height, uint32_t bpp) {
	fb = fbaddr;
	fb_width = width;
	fb_height = height;
	fb_pitch = pitch;
	bytesperpixel = bpp / 8;

	/*int res = kernel_alloc_in_place(fbaddr, fb_pitch * fb_height * bytesperpixel);
	char buffer[64];
	stbsp_sprintf(buffer, "Framebuffer allocation in place ... %s\n", res == NULL ? "fail" : "OK");
	com1_write_string(buffer);*/

	SET_COLOR(clr_transparent, 0, 0, 0, 0);
	SET_COLOR(clr_red, 255, 0, 0, 255);
	SET_COLOR(clr_green, 0, 255, 0, 255);
	SET_COLOR(clr_blue, 0, 0, 255, 255);
	SET_COLOR(clr_white, 255, 255, 255, 255);

	//graphics_clear(clr_red);

	graphics_initialized = 1;
}