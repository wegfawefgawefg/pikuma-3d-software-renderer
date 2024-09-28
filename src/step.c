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

void step_meta_triangle(MetaTriangle *mt)
{
    step_point(&mt->t.p1, &mt->vel.p1);
    step_point(&mt->t.p2, &mt->vel.p2);
    step_point(&mt->t.p3, &mt->vel.p3);

    point_reverse_on_walls(&mt->t.p1, &mt->vel.p1);
    point_reverse_on_walls(&mt->t.p2, &mt->vel.p2);
    point_reverse_on_walls(&mt->t.p3, &mt->vel.p3);
}

void step(State *state)
{
    for (int i = 0; i < NUM_TRIANGLES; i++)
    {
        step_meta_triangle(&state->triangles[i]);
    }
}
