#include "state.h"
#include "utils.h"
#include "globals.h"

State *new_state(void)
{
    State *state = malloc(sizeof(State));
    if (!state)
    {
        return NULL;
    }

    state->quit = false;
    state->scale = 10.0f;
    state->camera_pos = vec3_create(126, 0, -38);

    state->pointer_pos = vec2_create(0, 0);

    state->ripple_center = vec2_create(0, 0);
    state->ripple_magnitude = 0.0f;

    return state;
}

void free_state(State *state)
{
    free(state);
}