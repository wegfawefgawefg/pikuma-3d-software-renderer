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
    // Meshes
    Mesh *gba_mesh;
    Mesh *cube_mesh;
    Mesh *quad_mesh;
    Mesh *triangle_mesh;

    // Textures
    PixelBuffer *gba_texture;
    PixelBuffer *manhat_texture;
    PixelBuffer *pointer_pixel_buffer;
    PixelBuffer *charmap_white;
    PixelBuffer *triangle_up_texture;

    // Animated Textures
    MultiFramePixelBuffer *earth_mfpb;
} Assets;

Assets *assets_load(void);
void assets_free(Assets *assets);

#endif // ASSETS_H