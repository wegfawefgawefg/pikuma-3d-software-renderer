#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>

#include <SDL2/SDL.h>

#include "pixel_buffer.h"
#include "stb_image.h"
#include "mesh.h"

////////////////////////////////////////////////////////////////////////////////
// MISC
////////////////////////////////////////////////////////////////////////////////

typedef struct // sized sdl texture
{
    SDL_Texture *texture;
    int width;
    int height;
} SizedSDLTexture;

SizedSDLTexture *sized_sdl_texture_load(const char *filename, SDL_Renderer *renderer);
void sized_sdl_texture_free(SizedSDLTexture *sized_sdl_texture);

Mesh *mesh_load_from_obj(const char *filename);

////////////////////////////////////////////////////////////////////////////////
// Textures Management
////////////////////////////////////////////////////////////////////////////////
typedef struct TexturesEntry
{
    char *path;
    char *filename;
    PixelBuffer *texture;
    struct TexturesEntry *next;
} TexturesEntry;

typedef struct
{
    TexturesEntry *entries;
    TexturesEntry *last;
    int num_entries;
} Textures;

// Interface
//// Basic
Textures *textures_new(void);
void textures_free(Textures *textures);

//// Crud
int textures_add(Textures *textures, const char *path, const char *filename);
PixelBuffer *textures_get(Textures *textures, const char *filename);
int textures_load_from_directory(Textures *textures, const char *directory_path);

//// Info
void textures_print(Textures *textures);

////////////////////////////////////////////////////////////////////////////////
// Assets
////////////////////////////////////////////////////////////////////////////////

// defs for the assets, such as gba overlay, gba power light, pointer
typedef struct
{
    // Meshes
    Mesh *gba_mesh;
    Mesh *cube_mesh;
    Mesh *quad_mesh;
    Mesh *triangle_mesh;

    // Textures: linked list of PixelBuffers
    Textures *textures;

    // Animated Textures
    MultiFramePixelBuffer *earth_mfpb;
} Assets;

Assets *assets_new(void);
Assets *assets_load(void);
void assets_free(Assets *assets);

#endif // ASSETS_H