#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "primitives.h"

typedef struct
{
    bool quit;

    Vec3 camera_pos;
    float scale;

    Vec2 pointer_pos;

    Vec2 ripple_center;
    float ripple_magnitude;

} State;

State *new_state(void);
void free_state(State *state);

#endif // STATE_H