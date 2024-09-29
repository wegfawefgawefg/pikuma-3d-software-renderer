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

//////////////////////// QUADS ////////////////////////
typedef struct
{
    Quad *quads;
    int count;
} Quads;

Quads *create_quads_cube(Vec3 center, float size);
Quads *create_quads_pyramid(Vec3 center, float base_size, float height);
Quads *create_quads_prism(Vec3 center, float base_width, float base_depth, float height);
Quads *create_quads_octahedron(Vec3 center, float size);
Quads *create_quads_arrow(float length, float width);

void destroy_quads(Quads *quads);

void shift_quad(Quad *quad, Vec3 shift);
void shift_quads(Quads *quads, Vec3 shift);
void rotate_quad(Quad *quad, Vec3 center, Vec3 rotation);
void rotate_quads(Quads *quads, Vec3 center, Vec3 rotation);

Vec3 get_quad_center(Quad *quad);
Vec3 get_quad_normal(Quad *quad);
Vec3 *get_quads_normals(Quads *quads);

int compare_quads(const void *a, const void *b);
void zsort_quads(Quads *quads);

#endif // PRIMITIVES_H