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

PixelBuffer *pixel_buffer_new(int width, int height);
void pixel_buffer_free(PixelBuffer *pb);
void copy_to_texture(PixelBuffer *pb, SDL_Texture *texture);

void pixel_buffer_set(PixelBuffer *pb, int x, int y, uint32_t color);
void pixel_buffer_set_alpha(PixelBuffer *pb, int x, int y, uint32_t color);
uint32_t pixel_buffer_get(PixelBuffer *pb, int x, int y);
void clear_pixel_buffer(PixelBuffer *pb, uint32_t color);
void fade_pixel_buffer(PixelBuffer *pb, uint8_t amount);
void pixel_buffer_fill(PixelBuffer *pb, uint32_t color);

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
PixelBuffer *pixelbuffer_load_from_png(const char *path);
void draw_outline(PixelBuffer *pb, uint32_t color);
IVec2 get_center_of_pixelbuffer(PixelBuffer *pb);

// multiframe pixel buffer
typedef struct
{
    int num_frames;
    int current_frame;
    PixelBuffer **frames;
} MultiFramePixelBuffer;

MultiFramePixelBuffer *mfpb_new(int num_frames, int width, int height);
void mfpb_free(MultiFramePixelBuffer *mfpb);
void mfpb_next_frame(MultiFramePixelBuffer *mfpb);
MultiFramePixelBuffer *mfpb_load_from_gif(const char *filename);

#endif // PIXEL_BUFFER_H