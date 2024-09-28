#ifndef UTILS_H
#define UTILS_H

#include "primitives.h"

#include <SDL2/SDL.h>

#define SWAP(a, b)          \
    do                      \
    {                       \
        typeof(a) temp = a; \
        a = b;              \
        b = temp;           \
    } while (0)
float rand_range(float min, float max);
float rand_range_int(int min, int max);

// need define aboves
int imin(int a, int b);
int imax(int a, int b);

#endif // UTILS_H