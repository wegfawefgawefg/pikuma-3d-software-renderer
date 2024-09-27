#include "draw.h"
#include "primitives.h"
#include "utils.h"

void draw_line(PixelBuffer *pb, int x0, int y0, int x1, int y1, uint32_t color)
{
    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;

    int err = dx - dy;
    int e2;

    while (1)
    {
        set_pixel(pb, x0, y0, color);

        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_rect(PixelBuffer *pb, int x, int y, int w, int h, uint32_t color)
{
    for (int i = x; i < x + w; i++)
    {
        for (int j = y; j < y + h; j++)
        {
            set_pixel(pb, i, j, color);
        }
    }
}

void draw_rect_lines(PixelBuffer *pb, int x, int y, int w, int h, uint32_t color)
{
    draw_line(pb, x, y, x + w, y, color);
    draw_line(pb, x + w, y, x + w, y + h, color);
    draw_line(pb, x + w, y + h, x, y + h, color);
    draw_line(pb, x, y + h, x, y, color);
}

void draw_triangle_lines(PixelBuffer *pb, Triangle t, uint32_t color)
{
    draw_line(pb, t.p1.x, t.p1.y, t.p2.x, t.p2.y, color);
    draw_line(pb, t.p2.x, t.p2.y, t.p3.x, t.p3.y, color);
    draw_line(pb, t.p3.x, t.p3.y, t.p1.x, t.p1.y, color);
}

// //////////////////////// COMPOUND DRAWING FUNCTIONS ////////////////////////
void draw_cursor(PixelBuffer *pb, int x, int y, int size, uint32_t color)
{
    draw_line(pb, x - size, y, x + size, y, color);
    draw_line(pb, x, y - size, x, y + size, color);
}