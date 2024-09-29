#include "assets.h"
#include <SDL2/SDL_image.h>

SizedSDLTexture *load_sized_sdl_texture(const char *filename, SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_Load(filename);
    if (!surface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    SizedSDLTexture *sized_sdl_texture = (SizedSDLTexture *)malloc(sizeof(SizedSDLTexture));
    sized_sdl_texture->texture = texture;
    sized_sdl_texture->width = surface->w;
    sized_sdl_texture->height = surface->h;

    SDL_FreeSurface(surface);

    return sized_sdl_texture;
}

void destroy_sized_sdl_texture(SizedSDLTexture *sized_sdl_texture)
{
    SDL_DestroyTexture(sized_sdl_texture->texture);
    free(sized_sdl_texture);
}

// typedef struct
// {
//     SizedSDLTexture *gba_overlay;
//     SizedSDLTexture *gba_power_light;
//     SizedSDLTexture *pointer_sized_sdl_texture;
//     PixelBuffer *pointer_pixel_buffer;
// } Assets;

Assets *load_assets(SDL_Renderer *renderer)
{
    Assets *assets = (Assets *)malloc(sizeof(Assets));

    assets->gba_overlay = load_sized_sdl_texture("./assets/textures/Clear-Purple-Shell-Game-Boy-Advance.png", renderer);
    if (!assets->gba_overlay)
    {
        printf("Failed to load gba overlay\n");
        return NULL;
    }

    assets->gba_power_light = load_sized_sdl_texture("./assets/textures/gbalight.png", renderer);
    if (!assets->gba_power_light)
    {
        printf("Failed to load gba power light\n");
        return NULL;
    }

    assets->pointer_sized_sdl_texture = load_sized_sdl_texture("./assets/textures/pointer.png", renderer);
    if (!assets->pointer_sized_sdl_texture)
    {
        printf("Failed to load pointer\n");
        return NULL;
    }

    assets->pointer_pixel_buffer = load_pixelbuffer_from_png("./assets/textures/pointer.png");
    if (!assets->pointer_pixel_buffer)
    {
        printf("Failed to load pointer into pixel buffer\n");
        return NULL;
    }

    return assets;
}

void destroy_assets(Assets *assets)
{
    destroy_sized_sdl_texture(assets->gba_overlay);
    destroy_sized_sdl_texture(assets->gba_power_light);
    destroy_sized_sdl_texture(assets->pointer_sized_sdl_texture);
    destroy_pixel_buffer(assets->pointer_pixel_buffer);
    free(assets);
}
