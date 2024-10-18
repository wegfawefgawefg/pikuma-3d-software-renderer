#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>

#include <SDL2/SDL.h>

#include "pixel_buffer.h"
#include "stb_image.h"
#include "mesh.h"

typedef struct // sized sdl texture
{
    SDL_Texture *texture;
    int width;
    int height;
} SizedSDLTexture;

SizedSDLTexture *sized_sdl_texture_load(const char *filename, SDL_Renderer *renderer);
void sized_sdl_texture_free(SizedSDLTexture *sized_sdl_texture);

Mesh *mesh_load_from_obj(const char *filename);

// defs for the assets, such as gba overlay, gba power light, pointer
typedef struct
{
    SizedSDLTexture *gba_overlay;
    SizedSDLTexture *gba_power_light;
    SizedSDLTexture *pointer_sized_sdl_texture;

    PixelBuffer *gba_texture;
    PixelBuffer *manhat_texture;
    PixelBuffer *pointer_pixel_buffer;
    PixelBuffer *charmap_white;
    PixelBuffer *triangle_up_texture;

    Mesh *gba_mesh;
    Mesh *jet_plane_mesh;
    Mesh *cube_mesh;
    Mesh *quad_mesh;
    Mesh *triangle_mesh;
} Assets;

Assets *assets_load(SDL_Renderer *renderer);
void assets_free(Assets *assets);

#endif // ASSETS_H