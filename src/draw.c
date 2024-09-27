#include "draw.h"
#include "primitives.h"
#include "utils.h"

void draw(PixelBuffer *pb, State *state)
{
    IPoint2D mousePos = get_mouse_pos();
    draw_cursor(pb, mousePos.x, mousePos.y, 10, 0xFFFFFFFF);
}