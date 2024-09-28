#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL2/SDL.h>

#include "vec2.h"

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} IRect;

typedef struct
{
    Vec2 p1;
    Vec2 p2;
    Vec2 p3;
} Triangle;

typedef struct
{
    Vec2 p1;
    Vec2 p2;
} Line;

typedef struct
{
    Vec2 center;
    float radius;
} Circle;

#endif // PRIMITIVES_H