#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "primitives.h"

typedef struct
{
    bool quit;

    Vec3 camera_pos;
    Vec3 camera_target;
    Vec3 camera_up;

    float scale;

    Vec2 pointer_pos;

} State;

State *new_state(void);
void free_state(State *state);

#endif // STATE_H