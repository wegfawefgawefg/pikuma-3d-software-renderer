#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vec3.h"

typedef struct Light
{
    Vec3 pos;
    uint32_t color;
    float brightness;
} Light;

Light light_new(Vec3 pos, uint32_t color, float brightness);

#endif