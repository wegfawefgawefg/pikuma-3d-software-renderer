#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL2/SDL.h>

#include "vec2.h"
#include "vec3.h"

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} IRect;

typedef struct
{
    Vec3 p1;
    Vec3 p2;
    Vec3 p3;
    Vec3 p4;
} Quad;

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

typedef struct
{
    Quad *quads;
    int count;
} Quads;

Quads *create_cube(Vec3 center, float size);
void rotate_quad_around_axis(Quad *quad, Vec3 axis, float degrees);
void rotate_quads(Quads *quads, Vec3 axis, float degrees);
Vec3 get_quad_normal(Quad *quad);
Vec3 *get_quads_normals(Quads *quads);

#endif // PRIMITIVES_H