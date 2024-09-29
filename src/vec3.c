#include "vec3.h"
#include <math.h>

// Vec3 implementations (float)
Vec3 vec3_create(float x, float y, float z)
{
    return (Vec3){x, y, z};
}

Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 vec3_mul(Vec3 v, float scalar)
{
    return (Vec3){v.x * scalar, v.y * scalar, v.z * scalar};
}

Vec3 vec3_div(Vec3 v, float scalar)
{
    return (Vec3){v.x / scalar, v.y / scalar, v.z / scalar};
}

float vec3_dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

float vec3_length(Vec3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 vec3_normalize(Vec3 v)
{
    float len = vec3_length(v);
    if (len != 0)
    {
        return vec3_div(v, len);
    }
    return v;
}

// vec3 rotate point around pivot
Vec3 vec3_rotate_point_around_axis(Vec3 point, Vec3 axis, float degrees)
{
    // Convert degrees to radians
    float radians = degrees * (M_PI / 180.0f);
    float cos_r = cosf(radians);
    float sin_r = sinf(radians);

    // Translate point to origin
    float translated_x = point.x - axis.x;
    float translated_y = point.y - axis.y;
    float translated_z = point.z - axis.z;

    // Rotate the point
    float rotated_x = translated_x * cos_r - translated_y * sin_r;
    float rotated_y = translated_x * sin_r + translated_y * cos_r;

    // Translate back
    return vec3_create(rotated_x + axis.x, rotated_y + axis.y, translated_z);
}

// IVec3 implementations (integer)
IVec3 ivec3_create(int x, int y, int z)
{
    return (IVec3){x, y, z};
}

IVec3 ivec3_add(IVec3 a, IVec3 b)
{
    return (IVec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

IVec3 ivec3_sub(IVec3 a, IVec3 b)
{
    return (IVec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

IVec3 ivec3_mul(IVec3 v, int scalar)
{
    return (IVec3){v.x * scalar, v.y * scalar, v.z * scalar};
}

IVec3 ivec3_div(IVec3 v, int scalar)
{
    return (IVec3){v.x / scalar, v.y / scalar, v.z / scalar};
}

int ivec3_dot(IVec3 a, IVec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

IVec3 ivec3_cross(IVec3 a, IVec3 b)
{
    return (IVec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}