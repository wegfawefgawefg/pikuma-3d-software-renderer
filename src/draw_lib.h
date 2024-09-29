#ifndef DRAW_LIB_H
#define DRAW_LIB_H

#include <SDL2/SDL.h>

#include "pixel_buffer.h"

//////////////////////// PRIMITIVE DRAWING FUNCTIONS ////////////////////////
void draw_line(PixelBuffer *pb, int x0, int y0, int x1, int y1, uint32_t color);
void draw_rect(PixelBuffer *pb, int x, int y, int w, int h, uint32_t color);
void draw_rect_lines(PixelBuffer *pb, int x, int y, int w, int h, uint32_t color);
void draw_triangle_lines(PixelBuffer *pb, Triangle t, uint32_t color);
void draw_triangle(PixelBuffer *pb, Triangle t, uint32_t color);
void draw_grid(PixelBuffer *pb, IVec2 start, IVec2 end, int spacing, uint32_t color);
void draw_grid_dots(PixelBuffer *pb, IVec2 start, IVec2 end, int spacing, uint32_t color);
void draw_checkerboard(PixelBuffer *pb, IVec2 start, IVec2 end, int spacing, uint32_t color1, uint32_t color2);
void draw_cursor(PixelBuffer *pb, int x, int y, int size, uint32_t color);

//////////////////////// ORTHOGRAPHIC PROJECTION ////////////////////////
void draw_ortho_quad_lines(PixelBuffer *pb, Quad *quad, uint32_t color);
void draw_ortho_quad(PixelBuffer *pb, Quad *quad, uint32_t color);

#endif // DRAW_H
