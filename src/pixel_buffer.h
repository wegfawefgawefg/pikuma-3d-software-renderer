#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

#include <stdint.h>

#include <SDL2/SDL.h>

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

#endif // PIXEL_BUFFER_H