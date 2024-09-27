#ifndef DRAW_LIB_H
#define DRAW_LIB_H

#include <SDL2/SDL.h>

#include "pixel_buffer.h"

//////////////////////// PRIMITIVE DRAWING FUNCTIONS ////////////////////////
void draw_line(PixelBuffer *pb, int x0, int y0, int x1, int y1, uint32_t color);
void draw_rect(PixelBuffer *pb, int x, int y, int w, int h, uint32_t color);
void draw_rect_lines(PixelBuffer *pb, int x, int y, int w, int h, uint32_t color);
void draw_triangle_lines(PixelBuffer *pb, Triangle t, uint32_t color);

void draw_cursor(PixelBuffer *pb, int x, int y, int size, uint32_t color);

#endif // DRAW_H
