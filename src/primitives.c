#include "primitives.h"
#include "vec3.h"

// make sure the normals are facing outward
Quads *create_quad_cube(Vec3 center, float size)
{
    Quads *quads = (Quads *)malloc(sizeof(Quads));
    quads->count = 6;
    quads->quads = (Quad *)malloc(sizeof(Quad) * quads->count);

    float half_size = size / 2.0f;

    // front
    quads->quads[0].p1 = vec3_create(center.x - half_size, center.y - half_size, center.z + half_size);
    quads->quads[0].p2 = vec3_create(center.x + half_size, center.y - half_size, center.z + half_size);
    quads->quads[0].p3 = vec3_create(center.x + half_size, center.y + half_size, center.z + half_size);
    quads->quads[0].p4 = vec3_create(center.x - half_size, center.y + half_size, center.z + half_size);

    // back
    quads->quads[1].p1 = vec3_create(center.x + half_size, center.y - half_size, center.z - half_size);
    quads->quads[1].p2 = vec3_create(center.x - half_size, center.y - half_size, center.z - half_size);
    quads->quads[1].p3 = vec3_create(center.x - half_size, center.y + half_size, center.z - half_size);
    quads->quads[1].p4 = vec3_create(center.x + half_size, center.y + half_size, center.z - half_size);

    // left
    quads->quads[2].p1 = vec3_create(center.x - half_size, center.y - half_size, center.z - half_size);
    quads->quads[2].p2 = vec3_create(center.x - half_size, center.y - half_size, center.z + half_size);
    quads->quads[2].p3 = vec3_create(center.x - half_size, center.y + half_size, center.z + half_size);
    quads->quads[2].p4 = vec3_create(center.x - half_size, center.y + half_size, center.z - half_size);

    // right
    quads->quads[3].p1 = vec3_create(center.x + half_size, center.y - half_size, center.z + half_size);
    quads->quads[3].p2 = vec3_create(center.x + half_size, center.y - half_size, center.z - half_size);
    quads->quads[3].p3 = vec3_create(center.x + half_size, center.y + half_size, center.z - half_size);
    quads->quads[3].p4 = vec3_create(center.x + half_size, center.y + half_size, center.z + half_size);

    // top
    quads->quads[4].p1 = vec3_create(center.x - half_size, center.y + half_size, center.z + half_size);
    quads->quads[4].p2 = vec3_create(center.x + half_size, center.y + half_size, center.z + half_size);
    quads->quads[4].p3 = vec3_create(center.x + half_size, center.y + half_size, center.z - half_size);
    quads->quads[4].p4 = vec3_create(center.x - half_size, center.y + half_size, center.z - half_size);

    // bottom
    quads->quads[5].p1 = vec3_create(center.x - half_size, center.y - half_size, center.z - half_size);
    quads->quads[5].p2 = vec3_create(center.x + half_size, center.y - half_size, center.z - half_size);
    quads->quads[5].p3 = vec3_create(center.x + half_size, center.y - half_size, center.z + half_size);
    quads->quads[5].p4 = vec3_create(center.x - half_size, center.y - half_size, center.z + half_size);

    return quads;
}

void rotate_quad_around_axis(Quad *quad, Vec3 axis, float degrees)
{
    quad->p1 = vec3_rotate_point_around_axis(quad->p1, axis, degrees);
    quad->p2 = vec3_rotate_point_around_axis(quad->p2, axis, degrees);
    quad->p3 = vec3_rotate_point_around_axis(quad->p3, axis, degrees);
    quad->p4 = vec3_rotate_point_around_axis(quad->p4, axis, degrees);
}

void rotate_quads(Quads *quads, Vec3 axis, float degrees)
{
    for (int i = 0; i < quads->count; i++)
    {
        rotate_quad_around_axis(&quads->quads[i], axis, degrees);
    }
}

Vec3 get_quad_normal(Quad *quad)
{
    Vec3 v1 = vec3_sub(quad->p2, quad->p1);
    Vec3 v2 = vec3_sub(quad->p3, quad->p1);
    Vec3 cross = vec3_cross(v1, v2);
    return vec3_normalize(cross);
}

Vec3 *get_quads_normals(Quads *quads)
{
    Vec3 *normals = (Vec3 *)malloc(sizeof(Vec3) * quads->count);
    for (int i = 0; i < quads->count; i++)
    {
        normals[i] = get_quad_normal(&quads->quads[i]);
    }
    return normals;
}