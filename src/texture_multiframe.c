#include "texture_multiframe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gif_lib.h>
#include "stb_image.h"

#include "texture.h"
#include "utils.h"

// Helper function to convert RGB to ARGB format
static uint32_t RGBToRGBA(int r, int g, int b, int a)
{
    // if the image is black return transparent
    if (r == 0 && g == 0 && b == 0)
    {
        return 0;
    }
    return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | (uint32_t)a;
}

// Helper function to get pixel color from color index
static uint32_t GetColorFromIndex(const GifFileType *gif, int colorIndex)
{
    ColorMapObject *colorMap = gif->SColorMap;
    if (colorMap == NULL || colorIndex >= colorMap->ColorCount)
    {
        // Return transparent if color map is not available or index is out of bounds
        return RGBToRGBA(0, 0, 0, 0);
    }
    GifColorType color = colorMap->Colors[colorIndex];
    return RGBToRGBA(color.Red, color.Green, color.Blue, 255);
}

// mfpb new
MultiFrameTexture *mfpb_new(int num_frames, int width, int height)
{
    MultiFrameTexture *mfpb = (MultiFrameTexture *)malloc(sizeof(MultiFrameTexture));
    if (mfpb == NULL)
    {
        return NULL;
    }

    mfpb->num_frames = num_frames;
    mfpb->current_frame = 0;
    mfpb->frames = (Texture **)malloc(num_frames * sizeof(Texture *));
    if (mfpb->frames == NULL)
    {
        free(mfpb);
        return NULL;
    }

    for (int i = 0; i < num_frames; i++)
    {
        mfpb->frames[i] = texture_new(width, height);
        if (mfpb->frames[i] == NULL)
        {
            mfpb_free(mfpb);
            return NULL;
        }
    }

    return mfpb;
}

// mfpb free
void mfpb_free(MultiFrameTexture *mfpb)
{
    if (mfpb == NULL)
    {
        return;
    }

    for (int i = 0; i < mfpb->num_frames; i++)
    {
        texture_free(mfpb->frames[i]);
    }

    free(mfpb->frames);
    free(mfpb);
}

// mfpb next frame
void mfpb_next_frame(MultiFrameTexture *mfpb)
{
    mfpb->current_frame = (mfpb->current_frame + 1) % mfpb->num_frames;
}

MultiFrameTexture *mfpb_load_from_gif(const char *filename)
{
    char path[512];
    snprintf(path, sizeof(path), "./assets/animated_textures/%s", filename);

    int error = 0;
    GifFileType *gif = DGifOpenFileName(path, &error);
    if (gif == NULL)
    {
        fprintf(stderr, "Failed to open GIF file %s: %s\n", filename, GifErrorString(error));
        return NULL;
    }

    if (DGifSlurp(gif) == GIF_ERROR)
    {
        fprintf(stderr, "Failed to read GIF file %s: %s\n", filename, GifErrorString(gif->Error));
        DGifCloseFile(gif, &error);
        return NULL;
    }

    int num_frames = gif->ImageCount;
    if (num_frames == 0)
    {
        fprintf(stderr, "No frames found in GIF file %s\n", filename);
        DGifCloseFile(gif, &error);
        return NULL;
    }

    // Assuming all frames have the same dimensions as the logical screen
    int width = gif->SWidth;
    int height = gif->SHeight;

    MultiFrameTexture *mfpb = mfpb_new(num_frames, width, height);
    if (mfpb == NULL)
    {
        fprintf(stderr, "Failed to allocate MultiFrameTexture\n");
        DGifCloseFile(gif, &error);
        return NULL;
    }

    // Initialize a base frame buffer to handle disposal methods
    Texture *base_buffer = texture_new(width, height);
    if (base_buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate base Texture\n");
        mfpb_free(mfpb);
        DGifCloseFile(gif, &error);
        return NULL;
    }
    texture_clear(base_buffer);

    // Iterate over each frame
    for (int i = 0; i < num_frames; i++)
    {
        SavedImage *frame = &gif->SavedImages[i];
        GifImageDesc *imageDesc = &frame->ImageDesc;

        // Handle frame position
        int frameLeft = imageDesc->Left;
        int frameTop = imageDesc->Top;
        int frameWidth = imageDesc->Width;
        int frameHeight = imageDesc->Height;

        // Create a temporary buffer to hold the frame
        Texture *temp_buffer = texture_new(width, height);
        if (temp_buffer == NULL)
        {
            fprintf(stderr, "Failed to allocate temp Texture for frame %d\n", i);
            texture_free(base_buffer);
            mfpb_free(mfpb);
            DGifCloseFile(gif, &error);
            return NULL;
        }

        // Copy the base buffer to temp_buffer
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint32_t color = texture_get(base_buffer, x, y);
                texture_set(temp_buffer, x, y, color);
            }
        }

        // Get the color map for this frame
        ColorMapObject *colorMap = frame->ImageDesc.ColorMap ? frame->ImageDesc.ColorMap : gif->SColorMap;
        if (colorMap == NULL)
        {
            fprintf(stderr, "No color map found for frame %d\n", i);
            texture_free(temp_buffer);
            texture_free(base_buffer);
            mfpb_free(mfpb);
            DGifCloseFile(gif, &error);
            return NULL;
        }

        // Iterate over each pixel in the frame
        for (int y = 0; y < frameHeight; y++)
        {
            for (int x = 0; x < frameWidth; x++)
            {
                int pixelIndex = y * frameWidth + x;
                GifByteType colorIndex = frame->RasterBits[pixelIndex];

                // Handle transparency if present
                uint32_t color = GetColorFromIndex(gif, colorIndex);

                // Check if this pixel is transparent
                // This example assumes that the transparent color index is set in the Graphics Control Extension
                // You may need to parse the Graphics Control Extension to get the transparent index
                // For simplicity, we'll skip handling transparency here

                texture_set(temp_buffer, frameLeft + x, frameTop + y, color);
            }
        }

        // Add the temp_buffer to MultiFrameTexture
        mfpb->frames[i] = temp_buffer;

        // Update the base buffer based on the disposal method
        // For simplicity, we'll assume disposal method 1 (do not dispose)
        // Handling all disposal methods requires more complex logic
        // You can refer to the GIF89a specification for details

        // In this example, we overwrite the base buffer with the current frame
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint32_t color = texture_get(temp_buffer, x, y);
                texture_set(base_buffer, x, y, color);
            }
        }
    }

    texture_free(base_buffer);
    DGifCloseFile(gif, &error);
    return mfpb;
}
