#include "step.h"
#include "globals.h"

void step_point(Vec2 *point, Vec2 *velocity)
{
    point->x += velocity->x;
    point->y += velocity->y;
}

void point_reverse_on_walls(Vec2 *point, Vec2 *velocity)
{
    if (point->x < 0 || point->x > RENDER_WIDTH)
    {
        velocity->x = -velocity->x;
    }
    if (point->y < 0 || point->y > RENDER_HEIGHT)
    {
        velocity->y = -velocity->y;
    }
}

void step(State *state)
{
    // drop ripple center by 0.02 each frame
    // clamp to 0
    state->ripple_magnitude = fmax(0, state->ripple_magnitude - 0.02);
}
