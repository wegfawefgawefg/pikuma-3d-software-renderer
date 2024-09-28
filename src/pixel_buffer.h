#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <stdint.h>

#include <SDL2/SDL.h>

#include "vec2.h"

typedef struct
{
    int width;
    int height;
    uint32_t *pixels;
} PixelBuffer;

PixelBuffer *create_pixel_buffer(int width, int height);
void destroy_pixel_buffer(PixelBuffer *pb);
void copy_to_texture(PixelBuffer *pb, SDL_Texture *texture);

void set_pixel(PixelBuffer *pb, int x, int y, uint32_t color);
uint32_t get_pixel(PixelBuffer *pb, int x, int y);
void clear_pixel_buffer(PixelBuffer *pb, uint32_t color);
void fade_pixel_buffer(PixelBuffer *pb, uint8_t amount);
void blit(PixelBuffer *src, PixelBuffer *dst, int x, int y);
void blit_ex(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, float angle, Vec2 center_of_rotation);
void blit_dumb(PixelBuffer *src, PixelBuffer *dst, int x, int y);
PixelBuffer *rotate_pixelbuffer(PixelBuffer *src, float degrees);
IVec2 calculate_new_top_left(PixelBuffer *src, float degrees, Vec2 center_of_rotation);
void color_rotate(PixelBuffer *pb, float hue_shift);
int load_png_into_pixelbuffer(const char *filename, PixelBuffer *buffer);
void draw_outline(PixelBuffer *pb, uint32_t color);
IVec2 get_center_of_pixelbuffer(PixelBuffer *pb);

#endif // PIXEL_BUFFER_H