#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL2/SDL.h>

typedef struct
{
    float x;
    float y;
} Point2D;

typedef struct
{
    int x;
    int y;
} IPoint2D;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} IRect;

typedef struct
{
    Point2D p1;
    Point2D p2;
    Point2D p3;
} Triangle;

typedef struct
{
    Point2D p1;
    Point2D p2;
} Line;

typedef struct
{
    Point2D center;
    float radius;
} Circle;

#endif // PRIMITIVES_H