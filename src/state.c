#include "state.h"
#include "utils.h"
#include "globals.h"

void randomize_meta_triangle(MetaTriangle *mt)
{
    // put the triangle in random position on the screen
    mt->t.p1.x = rand_range_int(0, RENDER_WIDTH);
    mt->t.p1.y = rand_range_int(0, RENDER_HEIGHT);
    mt->t.p2.x = rand_range_int(0, RENDER_WIDTH);
    mt->t.p2.y = rand_range_int(0, RENDER_HEIGHT);
    mt->t.p3.x = rand_range_int(0, RENDER_WIDTH);
    mt->t.p3.y = rand_range_int(0, RENDER_HEIGHT);

    // random velocity
    const float MAX_VEL = 1.0f;
    mt->vel.p1.x = rand_range(-MAX_VEL, MAX_VEL);
    mt->vel.p1.y = rand_range(-MAX_VEL, MAX_VEL);
    mt->vel.p2.x = rand_range(-MAX_VEL, MAX_VEL);
    mt->vel.p2.y = rand_range(-MAX_VEL, MAX_VEL);
    mt->vel.p3.x = rand_range(-MAX_VEL, MAX_VEL);
    mt->vel.p3.y = rand_range(-MAX_VEL, MAX_VEL);

    // random color
    int r = rand_range_int(0, 255);
    int g = rand_range_int(0, 255);
    int b = rand_range_int(0, 255);
    mt->color = 0xFF000000 | (r << 16) | (g << 8) | b;
}

State *new_state(void)
{
    State *state = malloc(sizeof(State));
    state->quit = false;
    for (int i = 0; i < NUM_TRIANGLES; i++)
    {
        randomize_meta_triangle(&state->triangles[i]);
    }

    return state;
}