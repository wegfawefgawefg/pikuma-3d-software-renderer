#ifndef UTILS_H
#define UTILS_H

#include "primitives.h"

#include <SDL2/SDL.h>

IPoint2D get_mouse_pos(void);
float rand_range(float min, float max);
float rand_range_int(int min, int max);

#endif // UTILS_H