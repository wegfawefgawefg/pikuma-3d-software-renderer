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
    state->camera_pos = vec3_create(0, 0, -200);
    state->camera_target = vec3_create(0, 0, 0);
    state->camera_up = vec3_create(0, 1, 0);

    state->pointer_pos = vec2_create(0, 0);

    return state;
}

void free_state(State *state)
{
    free(state);
}