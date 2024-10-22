#ifndef TEXTURE_MULTIFRAME_H
#define TEXTURE_MULTIFRAME_H

#include <stdint.h>

#include <SDL2/SDL.h>

#include "vec2.h"
#include "texture.h"

// multiframe texture
typedef struct
{
    int num_frames;
    int current_frame;
    Texture **frames;
} MultiFrameTexture;

MultiFrameTexture *mfpb_new(int num_frames, int width, int height);
void mfpb_free(MultiFrameTexture *mfpb);
void mfpb_next_frame(MultiFrameTexture *mfpb);
MultiFrameTexture *mfpb_load_from_gif(const char *filename);

#endif