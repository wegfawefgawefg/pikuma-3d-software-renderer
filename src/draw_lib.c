#include "draw.h"
#include "primitives.h"
#include "utils.h"
#include "vec2.h"

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

static inline int edge_function(IVec2 a, IVec2 b, IVec2 p)
{
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

void draw_triangle(PixelBuffer *pb, Triangle t, uint32_t color)
{
    IVec2 p1 = vec2_to_ivec2(t.p1);
    IVec2 p2 = vec2_to_ivec2(t.p2);
    IVec2 p3 = vec2_to_ivec2(t.p3);

    // Compute bounding box
    int minX = imin(imin(p1.x, p2.x), p3.x);
    int minY = imin(imin(p1.y, p2.y), p3.y);
    int maxX = imax(imax(p1.x, p2.x), p3.x);
    int maxY = imax(imax(p1.y, p2.y), p3.y);

    // Clip against screen bounds
    minX = imax(minX, 0);
    minY = imax(minY, 0);
    maxX = imin(maxX, pb->width - 1);
    maxY = imin(maxY, pb->height - 1);

    // Determine triangle orientation
    int area = edge_function(p1, p2, p3);
    bool clockwise = area <= 0;

    IVec2 p;
    for (p.y = minY; p.y <= maxY; p.y++)
    {
        for (p.x = minX; p.x <= maxX; p.x++)
        {
            int w1 = edge_function(p1, p2, p);
            int w2 = edge_function(p2, p3, p);
            int w3 = edge_function(p3, p1, p);

            // If p is on or inside all edges, render pixel
            if (clockwise)
            {
                if (w1 <= 0 && w2 <= 0 && w3 <= 0)
                {
                    set_pixel_alpha(pb, p.x, p.y, color);
                }
            }
            else
            {
                if (w1 >= 0 && w2 >= 0 && w3 >= 0)
                {
                    set_pixel_alpha(pb, p.x, p.y, color);
                }
            }
        }
    }
}

// //////////////////////// COMPOUND DRAWING FUNCTIONS ////////////////////////
void draw_cursor(PixelBuffer *pb, int x, int y, int size, uint32_t color)
{
    draw_line(pb, x - size, y, x + size, y, color);
    draw_line(pb, x, y - size, x, y + size, color);
}

void draw_grid(PixelBuffer *pb, IVec2 start, IVec2 end, int spacing, uint32_t color)
{
    for (int x = start.x; x < end.x; x += spacing)
    {
        draw_line(pb, x, start.y, x, end.y, color);
    }
    for (int y = start.y; y < end.y; y += spacing)
    {
        draw_line(pb, start.x, y, end.x, y, color);
    }

    // draw the rightmost and bottommost lines
    draw_line(pb, end.x, start.y, end.x, end.y, color);
    draw_line(pb, start.x, end.y, end.x, end.y, color);
}

// draw a grid with dots instead of , puts a dot at each intersection
void draw_grid_dots(PixelBuffer *pb, IVec2 start, IVec2 end, int spacing, uint32_t color)
{
    for (int x = start.x; x < end.x + 1; x += spacing)
    {
        for (int y = start.y; y < end.y + 1; y += spacing)
        {
            set_pixel(pb, x, y, color);
        }
    }

    // draw the rightmost and bottommost line of dots
    for (int x = start.x; x < end.x; x += spacing)
    {
        set_pixel(pb, x, end.y, color);
    }

    for (int y = start.y; y < end.y; y += spacing)
    {
        set_pixel(pb, end.x, y, color);
    }

    // bottom right corner
    set_pixel(pb, end.x, end.y, color);
}

// draw_checkerboard
// takes in two colors and the size of the squares
//    draw_checkerboard(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, 0xFF333333, 0xFF444444);

void draw_checkerboard(PixelBuffer *pb, IVec2 start, IVec2 end, int spacing, uint32_t color1, uint32_t color2)
{
    for (int x = start.x; x < end.x + 1; x += spacing)
    {
        for (int y = start.y; y < end.y + 1; y += spacing)
        {
            if ((x / spacing + y / spacing) % 2 == 0)
            {
                draw_rect(pb, x, y, spacing, spacing, color1);
            }
            else
            {
                draw_rect(pb, x, y, spacing, spacing, color2);
            }
        }
    }
}

void draw_ortho_quad_lines(PixelBuffer *pb, Quad *quad, uint32_t color)
{
    draw_line(pb, quad->p1.x, quad->p1.y, quad->p2.x, quad->p2.y, color);
    draw_line(pb, quad->p2.x, quad->p2.y, quad->p3.x, quad->p3.y, color);
    draw_line(pb, quad->p3.x, quad->p3.y, quad->p4.x, quad->p4.y, color);
    draw_line(pb, quad->p4.x, quad->p4.y, quad->p1.x, quad->p1.y, color);
}

void draw_ortho_quad(PixelBuffer *pb, Quad *quad, uint32_t color)
{
    Triangle t1 = {vec2_create(quad->p1.x, quad->p1.y), vec2_create(quad->p2.x, quad->p2.y), vec2_create(quad->p3.x, quad->p3.y)};
    Triangle t2 = {vec2_create(quad->p1.x, quad->p1.y), vec2_create(quad->p3.x, quad->p3.y), vec2_create(quad->p4.x, quad->p4.y)};

    draw_triangle(pb, t1, color);
    draw_triangle(pb, t2, color);
}

// in this case the SFA is 2d verticies: x,y
void draw_points(PixelBuffer *pb, SFA *points, uint32_t color)
{
    for (int i = 0; i < points->length; i += 2)
    {
        int x = points->data[i];
        int y = points->data[i + 1];
        set_pixel(pb, x, y, color);
    }
}

// in this case the SFA is 2d verticies: x,y,x,y
void draw_tris(PixelBuffer *pb, SFA *verticies, SIA *indices, uint32_t color)
{
    for (int i = 0; i < indices->length; i += 3)
    {
        int idx1 = indices->data[i];
        int idx2 = indices->data[i + 1];
        int idx3 = indices->data[i + 2];

        Vec2 p1 = {verticies->data[idx1 * 2], verticies->data[idx1 * 2 + 1]};
        Vec2 p2 = {verticies->data[idx2 * 2], verticies->data[idx2 * 2 + 1]};
        Vec2 p3 = {verticies->data[idx3 * 2], verticies->data[idx3 * 2 + 1]};

        Triangle t = {p1, p2, p3};
        draw_triangle(pb, t, color);
    }
}

void draw_tris_with_colors(PixelBuffer *pb, SFA *verticies, SIA *indices, SU32A *colors)
{
    for (int i = 0; i < indices->length; i += 3)
    {
        int idx1 = indices->data[i];
        int idx2 = indices->data[i + 1];
        int idx3 = indices->data[i + 2];

        Vec2 p1 = {verticies->data[idx1 * 2], verticies->data[idx1 * 2 + 1]};
        Vec2 p2 = {verticies->data[idx2 * 2], verticies->data[idx2 * 2 + 1]};
        Vec2 p3 = {verticies->data[idx3 * 2], verticies->data[idx3 * 2 + 1]};

        Triangle t = {p1, p2, p3};
        uint32_t color = colors->data[i / 3];

        // print the color
        draw_triangle(pb, t, color);
    }
}

// // in the center of each try in red just draw the face index
void draw_tris_face_numbers(PixelBuffer *pb, PixelBuffer *charmap, SFA *verticies, SIA *indices, uint32_t size, uint32_t color)
{
    for (int i = 0; i < indices->length; i += 3)
    {
        int idx1 = indices->data[i];
        int idx2 = indices->data[i + 1];
        int idx3 = indices->data[i + 2];

        Vec2 p1 = {verticies->data[idx1 * 2], verticies->data[idx1 * 2 + 1]};
        Vec2 p2 = {verticies->data[idx2 * 2], verticies->data[idx2 * 2 + 1]};
        Vec2 p3 = {verticies->data[idx3 * 2], verticies->data[idx3 * 2 + 1]};

        Vec2 center = vec2_create((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3);
        char face_number[10];
        sprintf(face_number, "%d", i / 3);
        blit_string(pb, charmap, face_number, center.x, center.y, size, color);
    }
}

void draw_tris_with_colors_and_face_numbers(PixelBuffer *pb, PixelBuffer *charmap, SFA *verticies, SIA *indices, SU32A *colors, uint32_t size, uint32_t color)
{
    for (int i = 0; i < indices->length; i += 3)
    {
        int idx1 = indices->data[i];
        int idx2 = indices->data[i + 1];
        int idx3 = indices->data[i + 2];

        Vec2 p1 = {verticies->data[idx1 * 2], verticies->data[idx1 * 2 + 1]};
        Vec2 p2 = {verticies->data[idx2 * 2], verticies->data[idx2 * 2 + 1]};
        Vec2 p3 = {verticies->data[idx3 * 2], verticies->data[idx3 * 2 + 1]};

        Triangle t = {p1, p2, p3};
        uint32_t face_color = colors->data[i / 3];
        draw_triangle(pb, t, face_color);

        Vec2 center = vec2_create((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3);
        char face_number[10];
        sprintf(face_number, "%d", i / 3);
        blit_string(pb, charmap, face_number, center.x, center.y, size, color);
    }
}

// in this case the SFA is 2d verticies: x,y,x,y
void draw_tris_lines(PixelBuffer *pb, SFA *verticies, SIA *indices, uint32_t color)
{
    for (int i = 0; i < indices->length; i += 3)
    {
        int idx1 = indices->data[i];
        int idx2 = indices->data[i + 1];
        int idx3 = indices->data[i + 2];

        Vec2 p1 = {verticies->data[idx1 * 2], verticies->data[idx1 * 2 + 1]};
        Vec2 p2 = {verticies->data[idx2 * 2], verticies->data[idx2 * 2 + 1]};
        Vec2 p3 = {verticies->data[idx3 * 2], verticies->data[idx3 * 2 + 1]};

        Triangle t = {p1, p2, p3};
        draw_triangle_lines(pb, t, color);
    }
}
