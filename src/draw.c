#include "draw.h"
#include "primitives.h"
#include "input.h"
#include "colors.h"

void draw(PixelBuffer *pb, State *state)
{
    // draw a grey grid on the back
    // draw_grid_dots(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, 0xFF333333);
    // draw_checkerboard(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, 0xFF333333, 0xFF444444);
    draw_checkerboard(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20,
                      COLOR_GRAY_DARK,
                      COLOR_TRANS_PURPLE);

    IVec2 mousePos = get_mouse_pos();
    draw_cursor(pb, mousePos.x, mousePos.y, 10, 0xFFFFFFFF);

    for (int i = 0; i < NUM_TRIANGLES; i++)
    {
        MetaTriangle *mt = &state->triangles[i];
        draw_triangle(pb, mt->t, mt->color);
    }
}