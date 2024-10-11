#include "light.h"

Light light_new(Vec3 pos, uint32_t color, float brightness)
{
    Light light = {pos, color, brightness};
    return light;
}
