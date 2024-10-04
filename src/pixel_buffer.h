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
void set_pixel_alpha(PixelBuffer *pb, int x, int y, uint32_t color);
uint32_t get_pixel(PixelBuffer *pb, int x, int y);
void clear_pixel_buffer(PixelBuffer *pb, uint32_t color);
void fade_pixel_buffer(PixelBuffer *pb, uint8_t amount);

PixelBuffer *rotate_pixelbuffer(PixelBuffer *src, float degrees);
PixelBuffer *scale_pixelbuffer(PixelBuffer *src, Vec2 scale);

void blit(PixelBuffer *src, PixelBuffer *dst, int x, int y);
void blit_with_scale(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, Vec2 scale);
void blit_with_rotation(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, float angle, Vec2 center_of_rotation);
void blit_with_scale_and_rotation(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, Vec2 scale, float angle, Vec2 center_of_rotation);
void blit_dumb(PixelBuffer *src, PixelBuffer *dst, int x, int y);
void blit_letter(PixelBuffer *target_pb, PixelBuffer *letters_pb, uint8_t ascii_value, int x, int y, int size, uint32_t color);
void blit_string(PixelBuffer *target_pb, PixelBuffer *letters_pb, const char *str, int x, int y, int size, uint32_t color);

IVec2 calculate_new_top_left(PixelBuffer *src, float degrees, Vec2 center_of_rotation);
void color_rotate(PixelBuffer *pb, float hue_shift);
PixelBuffer *load_pixelbuffer_from_png(const char *filename);
void draw_outline(PixelBuffer *pb, uint32_t color);
IVec2 get_center_of_pixelbuffer(PixelBuffer *pb);

#endif // PIXEL_BUFFER_H