#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>

#include "pixel_buffer.h"
#include "stb_image.h"
#include <SDL2/SDL.h>

typedef struct // sized sdl texture
{
    SDL_Texture *texture;
    int width;
    int height;
} SizedSDLTexture;

SizedSDLTexture *load_sized_sdl_texture(const char *filename, SDL_Renderer *renderer);
void destroy_sized_sdl_texture(SizedSDLTexture *sized_sdl_texture);

// defs for the assets, such as gba overlay, gba power light, pointer
typedef struct
{
    SizedSDLTexture *gba_overlay;
    SizedSDLTexture *gba_power_light;
    SizedSDLTexture *pointer_sized_sdl_texture;
    PixelBuffer *pointer_pixel_buffer;
} Assets;

Assets *load_assets(SDL_Renderer *renderer);
void destroy_assets(Assets *assets);

#endif // ASSETS_H