#include "pixel_buffer.h"

PixelBuffer *create_pixel_buffer(int width, int height)
{
    PixelBuffer *pb = (PixelBuffer *)malloc(sizeof(PixelBuffer));
    pb->width = width;
    pb->height = height;
    pb->pixels = (uint32_t *)calloc(width * height, sizeof(uint32_t));
    return pb;
}

void destroy_pixel_buffer(PixelBuffer *pb)
{
    free(pb->pixels);
    free(pb);
}

void copy_to_texture(PixelBuffer *pb, SDL_Texture *texture)
{
    SDL_UpdateTexture(texture, NULL, pb->pixels, pb->width * sizeof(uint32_t));
}

void set_pixel(PixelBuffer *pb, int x, int y, uint32_t color)
{
    if (x < 0 || x >= pb->width || y < 0 || y >= pb->height)
    {
        return;
    }
    pb->pixels[y * pb->width + x] = color;
}

uint32_t get_pixel(PixelBuffer *pb, int x, int y)
{
    if (x < 0 || x >= pb->width || y < 0 || y >= pb->height)
    {
        return 0;
    }
    return pb->pixels[y * pb->width + x];
}

void clear_pixel_buffer(PixelBuffer *pb, uint32_t color)
{
    for (int i = 0; i < pb->width * pb->height; i++)
    {
        pb->pixels[i] = color;
    }
}

// just subtract amount from each color channel and alpha channel, colors are 4 bytes
void fade_pixel_buffer(PixelBuffer *pb, uint8_t amount)
{
    for (int i = 0; i < pb->width * pb->height; i++)
    {
        uint32_t color = pb->pixels[i];
        uint8_t *color_bytes = (uint8_t *)&color;
        for (int j = 0; j < 4; j++)
        {
            if (color_bytes[j] > amount)
            {
                color_bytes[j] -= amount;
            }
            else
            {
                color_bytes[j] = 0;
            }
        }
        pb->pixels[i] = color;
    }
}

void blit(PixelBuffer *src, PixelBuffer *dst, int x, int y)
{
    // determine the intersection bounds
    int x0 = x;
    int x1 = x + src->width;
    int y0 = y;
    int y1 = y + src->height;

    if (x0 < 0)
    {
        x0 = 0;
    }
    if (x1 > dst->width)
    {
        x1 = dst->width;
    }
    if (y0 < 0)
    {
        y0 = 0;
    }
    if (y1 > dst->height)
    {
        y1 = dst->height;
    }

    for (int j = y0; j < y1; j++)
    {
        for (int i = x0; i < x1; i++)
        {
            uint32_t color = get_pixel(src, i - x, j - y);
            set_pixel(dst, i, j, color);
        }
    }
}
