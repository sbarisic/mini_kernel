#pragma once

#ifndef GRAPHICS_EXTERN
#define GRAPHICS_EXTERN extern
#endif // !GRAPHICS_EXTERN

#define SET_COLOR(c, r, g, b, a) do { c.R = (r); c.G = (g); c.B = (b); c.A = (a); } while(0)

typedef struct {
	uint8_t B;
	uint8_t G;
	uint8_t R;
	uint8_t Padding;
} color_fb;

typedef struct {
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;
} color;

GRAPHICS_EXTERN color clr_transparent;
GRAPHICS_EXTERN color clr_red;
GRAPHICS_EXTERN color clr_green;
GRAPHICS_EXTERN color clr_blue;
GRAPHICS_EXTERN color clr_white;
GRAPHICS_EXTERN int32_t font_char_w;
GRAPHICS_EXTERN int32_t font_char_h;
GRAPHICS_EXTERN int32_t graphics_text_width;
GRAPHICS_EXTERN int32_t graphics_text_height;
GRAPHICS_EXTERN int32_t graphics_initialized;

void graphics_init(uint8_t* fbaddr, uint32_t pitch, uint32_t width, uint32_t height, uint32_t bpp);
void graphics_set_blend_color(color clr);
color graphics_get_blend_color();
void graphics_set_text_color(color clr);
color graphics_get_text_color();
void graphics_get_res(int* w, int* h);

void graphics_load_font(uint32_t* font_file);
void graphics_load_background(uint32_t* bg_file);
void graphics_clear(color clr);

void graphics_blit_image(int xoffset, int yoffset, int w, int h, color* image_file);
void graphics_blit_image2(int xoffset, int yoffset, int src_xoffset, int src_yoffset, int src_w, int src_h, int image_w, int image_h, color* image_data);

void graphics_putpixel2(int32_t x, int32_t y, color* clr);
void graphics_putline(int x0, int y0, int x1, int y1, color* clr);
void graphics_putcircle(int xm, int ym, int r, color* clr);

void graphics_blitchar(int32_t char_x, int32_t char_y, int32_t c);