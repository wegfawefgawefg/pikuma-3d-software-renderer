#include "vec2.h"
#include <math.h>

// Vec2 implementations (float)
Vec2 vec2_create(float x, float y)
{
    return (Vec2){x, y};
}

Vec2 vec2_add(Vec2 a, Vec2 b)
{
    return (Vec2){a.x + b.x, a.y + b.y};
}

Vec2 vec2_sub(Vec2 a, Vec2 b)
{
    return (Vec2){a.x - b.x, a.y - b.y};
}

Vec2 vec2_mul(Vec2 v, float scalar)
{
    return (Vec2){v.x * scalar, v.y * scalar};
}

float vec2_dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

float vec2_length(Vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vec2 vec2_normalize(Vec2 v)
{
    float len = vec2_length(v);
    if (len != 0)
    {
        return vec2_mul(v, 1.0f / len);
    }
    return v;
}

// IVec2 implementations (integer)
IVec2 ivec2_create(int x, int y)
{
    return (IVec2){x, y};
}

IVec2 vec2_to_ivec2(Vec2 v)
{
    return (IVec2){(int)v.x, (int)v.y};
}

IVec2 ivec2_add(IVec2 a, IVec2 b)
{
    return (IVec2){a.x + b.x, a.y + b.y};
}

IVec2 ivec2_sub(IVec2 a, IVec2 b)
{
    return (IVec2){a.x - b.x, a.y - b.y};
}

IVec2 ivec2_mul(IVec2 v, int scalar)
{
    return (IVec2){v.x * scalar, v.y * scalar};
}

int ivec2_dot(IVec2 a, IVec2 b)
{
    return a.x * b.x + a.y * b.y;
}