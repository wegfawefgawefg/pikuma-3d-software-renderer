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

int imin(int a, int b);
int imax(int a, int b);

void rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v);
void hsv_to_rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b);

#endif // UTILS_H