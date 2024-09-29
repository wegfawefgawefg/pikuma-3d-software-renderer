#include "primitives.h"
#include "vec3.h"
#include "utils.h"

Quads *create_quads_cube(Vec3 center, float size)
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

Quads *create_quads_pyramid(Vec3 center, float base_size, float height)
{
    Quads *quads = (Quads *)malloc(sizeof(Quads));
    quads->count = 5; // 4 triangular sides + 1 square base
    quads->quads = (Quad *)malloc(sizeof(Quad) * quads->count);

    float half_base = base_size / 2.0f;
    Vec3 apex = vec3_create(center.x, center.y + height, center.z);

    // Base
    quads->quads[0].p1 = vec3_create(center.x - half_base, center.y, center.z - half_base);
    quads->quads[0].p2 = vec3_create(center.x + half_base, center.y, center.z - half_base);
    quads->quads[0].p3 = vec3_create(center.x + half_base, center.y, center.z + half_base);
    quads->quads[0].p4 = vec3_create(center.x - half_base, center.y, center.z + half_base);

    // Front face
    quads->quads[1].p1 = vec3_create(center.x - half_base, center.y, center.z + half_base);
    quads->quads[1].p2 = vec3_create(center.x + half_base, center.y, center.z + half_base);
    quads->quads[1].p3 = apex;
    quads->quads[1].p4 = apex;

    // Right face
    quads->quads[2].p1 = vec3_create(center.x + half_base, center.y, center.z + half_base);
    quads->quads[2].p2 = vec3_create(center.x + half_base, center.y, center.z - half_base);
    quads->quads[2].p3 = apex;
    quads->quads[2].p4 = apex;

    // Back face
    quads->quads[3].p1 = vec3_create(center.x + half_base, center.y, center.z - half_base);
    quads->quads[3].p2 = vec3_create(center.x - half_base, center.y, center.z - half_base);
    quads->quads[3].p3 = apex;
    quads->quads[3].p4 = apex;

    // Left face
    quads->quads[4].p1 = vec3_create(center.x - half_base, center.y, center.z - half_base);
    quads->quads[4].p2 = vec3_create(center.x - half_base, center.y, center.z + half_base);
    quads->quads[4].p3 = apex;
    quads->quads[4].p4 = apex;

    return quads;
}

Quads *create_quads_prism(Vec3 center, float base_width, float base_depth, float height)
{
    Quads *quads = (Quads *)malloc(sizeof(Quads));
    quads->count = 5; // 2 triangular ends + 3 rectangular sides
    quads->quads = (Quad *)malloc(sizeof(Quad) * quads->count);

    float half_width = base_width / 2.0f;
    float half_depth = base_depth / 2.0f;
    float half_height = height / 2.0f;

    // Front triangular face
    quads->quads[0].p1 = vec3_create(center.x - half_width, center.y - half_height, center.z + half_depth);
    quads->quads[0].p2 = vec3_create(center.x + half_width, center.y - half_height, center.z + half_depth);
    quads->quads[0].p3 = vec3_create(center.x, center.y + half_height, center.z + half_depth);
    quads->quads[0].p4 = vec3_create(center.x, center.y + half_height, center.z + half_depth);

    // Back triangular face
    quads->quads[1].p1 = vec3_create(center.x + half_width, center.y - half_height, center.z - half_depth);
    quads->quads[1].p2 = vec3_create(center.x - half_width, center.y - half_height, center.z - half_depth);
    quads->quads[1].p3 = vec3_create(center.x, center.y + half_height, center.z - half_depth);
    quads->quads[1].p4 = vec3_create(center.x, center.y + half_height, center.z - half_depth);

    // Bottom rectangular face
    quads->quads[2].p1 = vec3_create(center.x - half_width, center.y - half_height, center.z - half_depth);
    quads->quads[2].p2 = vec3_create(center.x + half_width, center.y - half_height, center.z - half_depth);
    quads->quads[2].p3 = vec3_create(center.x + half_width, center.y - half_height, center.z + half_depth);
    quads->quads[2].p4 = vec3_create(center.x - half_width, center.y - half_height, center.z + half_depth);

    // Left rectangular face
    quads->quads[3].p1 = vec3_create(center.x - half_width, center.y - half_height, center.z - half_depth);
    quads->quads[3].p2 = vec3_create(center.x - half_width, center.y - half_height, center.z + half_depth);
    quads->quads[3].p3 = vec3_create(center.x, center.y + half_height, center.z + half_depth);
    quads->quads[3].p4 = vec3_create(center.x, center.y + half_height, center.z - half_depth);

    // Right rectangular face
    quads->quads[4].p1 = vec3_create(center.x + half_width, center.y - half_height, center.z + half_depth);
    quads->quads[4].p2 = vec3_create(center.x + half_width, center.y - half_height, center.z - half_depth);
    quads->quads[4].p3 = vec3_create(center.x, center.y + half_height, center.z - half_depth);
    quads->quads[4].p4 = vec3_create(center.x, center.y + half_height, center.z + half_depth);

    return quads;
}

Quads *create_quads_octahedron(Vec3 center, float size)
{
    Quads *quads = (Quads *)malloc(sizeof(Quads));
    quads->count = 8; // 8 triangular faces
    quads->quads = (Quad *)malloc(sizeof(Quad) * quads->count);

    float half_size = size / 2.0f;

    Vec3 top = vec3_create(center.x, center.y + half_size, center.z);
    Vec3 bottom = vec3_create(center.x, center.y - half_size, center.z);
    Vec3 front = vec3_create(center.x, center.y, center.z + half_size);
    Vec3 back = vec3_create(center.x, center.y, center.z - half_size);
    Vec3 right = vec3_create(center.x + half_size, center.y, center.z);
    Vec3 left = vec3_create(center.x - half_size, center.y, center.z);

    // Top four faces
    quads->quads[0].p1 = top;
    quads->quads[0].p2 = front;
    quads->quads[0].p3 = right;
    quads->quads[0].p4 = right;
    quads->quads[1].p1 = top;
    quads->quads[1].p2 = right;
    quads->quads[1].p3 = back;
    quads->quads[1].p4 = back;
    quads->quads[2].p1 = top;
    quads->quads[2].p2 = back;
    quads->quads[2].p3 = left;
    quads->quads[2].p4 = left;
    quads->quads[3].p1 = top;
    quads->quads[3].p2 = left;
    quads->quads[3].p3 = front;
    quads->quads[3].p4 = front;

    // Bottom four faces
    quads->quads[4].p1 = bottom;
    quads->quads[4].p2 = right;
    quads->quads[4].p3 = front;
    quads->quads[4].p4 = front;
    quads->quads[5].p1 = bottom;
    quads->quads[5].p2 = back;
    quads->quads[5].p3 = right;
    quads->quads[5].p4 = right;
    quads->quads[6].p1 = bottom;
    quads->quads[6].p2 = left;
    quads->quads[6].p3 = back;
    quads->quads[6].p4 = back;
    quads->quads[7].p1 = bottom;
    quads->quads[7].p2 = front;
    quads->quads[7].p3 = left;
    quads->quads[7].p4 = left;

    return quads;
}

Quads *create_quads_arrow(float length, float width)
{
    // make a cube as the base, and a prism as the point
    Quads *quads = create_quads_cube(vec3_create(0, 0, 0), width);
    Quads *prism = create_quads_prism(vec3_create(0, 0, 0), width * 1.4, width * 1.4, width);

    // rotate the prism around the x axis 90 degrees
    rotate_quads(prism, vec3_create(0, 0, 0), vec3_create(degrees_to_radians(90), 0, 0));

    // shift the prism to the top of the cube
    Vec3 shift = vec3_create(0, 0, width / 2.0f + length / 2.0f);
    shift_quads(prism, shift);
    // shift the prism back towards the cube like 0.2 of the width
    shift_quads(prism, vec3_mul(vec3_create(0, 0, -1), width * 0.3));

    // combine the two
    Quads *combined = (Quads *)malloc(sizeof(Quads));
    combined->count = quads->count + prism->count;
    combined->quads = (Quad *)malloc(sizeof(Quad) * combined->count);

    for (int i = 0; i < quads->count; i++)
    {
        combined->quads[i] = quads->quads[i];
    }

    for (int i = 0; i < prism->count; i++)
    {
        combined->quads[quads->count + i] = prism->quads[i];
    }

    // shift all of them over some multiple of width
    shift_quads(combined, vec3_mul(vec3_create(0, 0, -1), width * 1.6));

    destroy_quads(quads);
    destroy_quads(prism);

    return combined;
}

void destroy_quads(Quads *quads)
{
    free(quads->quads);
    free(quads);
}

void shift_quad(Quad *quad, Vec3 shift)
{
    quad->p1 = vec3_add(quad->p1, shift);
    quad->p2 = vec3_add(quad->p2, shift);
    quad->p3 = vec3_add(quad->p3, shift);
    quad->p4 = vec3_add(quad->p4, shift);
}
void shift_quads(Quads *quads, Vec3 shift)
{
    for (int i = 0; i < quads->count; i++)
    {
        shift_quad(&quads->quads[i], shift);
    }
}

void rotate_quad(Quad *quad, Vec3 center, Vec3 rotation)
{
    quad->p1 = vec3_rotate_point(quad->p1, center, rotation);
    quad->p2 = vec3_rotate_point(quad->p2, center, rotation);
    quad->p3 = vec3_rotate_point(quad->p3, center, rotation);
    quad->p4 = vec3_rotate_point(quad->p4, center, rotation);
}

void rotate_quads(Quads *quads, Vec3 center, Vec3 rotation)
{
    for (int i = 0; i < quads->count; i++)
    {
        rotate_quad(&quads->quads[i], center, rotation);
    }
}

Vec3 get_quad_center(Quad *quad)
{
    return vec3_div(vec3_add(vec3_add(quad->p1, quad->p2), vec3_add(quad->p3, quad->p4)), 4);
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

int compare_quads(const void *a, const void *b)
{
    Quad *quadA = (Quad *)a;
    Quad *quadB = (Quad *)b;

    Vec3 centerA = get_quad_center(quadA);
    Vec3 centerB = get_quad_center(quadB);

    // Sort from back to front
    if (centerA.z < centerB.z)
        return 1;
    if (centerA.z > centerB.z)
        return -1;
    return 0;
}

void zsort_quads(Quads *quads)
{
    qsort(quads->quads, quads->count, sizeof(Quad), compare_quads);
}
