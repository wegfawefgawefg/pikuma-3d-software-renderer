#include "primitives.h"
#include "globals.h"

float rand_range(float min, float max)
{
    // Generate a random float between 0 and 1
    float scale = rand() / (float)RAND_MAX;
    // Scale and shift it to the desired range
    return min + scale * (max - min);
}

float rand_range_int(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

int imin(int a, int b) { return (a < b) ? a : b; }
int imax(int a, int b) { return (a > b) ? a : b; }
