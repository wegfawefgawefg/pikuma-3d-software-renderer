#include "draw.h"
#include "primitives.h"
#include "input.h"

void draw(PixelBuffer *pb, State *state)
{
    IPoint2D mousePos = get_mouse_pos();
    draw_cursor(pb, mousePos.x, mousePos.y, 10, 0xFFFFFFFF);

    for (int i = 0; i < NUM_TRIANGLES; i++)
    {
        MetaTriangle *mt = &state->triangles[i];
        draw_triangle_lines(pb, mt->t, mt->color);
    }
}