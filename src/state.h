#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "primitives.h"

typedef struct
{
    Triangle t;
    Triangle vel;
    uint32_t color;
} MetaTriangle;

#define NUM_TRIANGLES 10

typedef struct
{
    bool quit;
    MetaTriangle triangles[NUM_TRIANGLES];

} State;

State *new_state(void);

#endif // STATE_H