#include "sfa.h"

#include <stdlib.h>
#include <math.h>
#include "vec3.h"

SFA *sfa_new(int length)
{
    SFA *sfa = (SFA *)malloc(sizeof(SFA));
    sfa->length = length;
    sfa->data = (float *)malloc(sizeof(float) * length);
    return sfa;
}

void sfa_free(SFA *sfa)
{
    free(sfa->data);
    free(sfa);
}

//////////////////////// TRANSFORM FUNCTIONS ////////////////////////

void sfa_rotate(SFA *sfa, Vec3 rotation)
{
    for (int i = 0; i < sfa->length; i += 3)
    {
        float x = sfa->data[i];
        float y = sfa->data[i + 1];
        float z = sfa->data[i + 2];

        // Rotate around X-axis
        float temp_y = y;
        float temp_z = z;
        y = temp_y * cosf(rotation.x) - temp_z * sinf(rotation.x);
        z = temp_y * sinf(rotation.x) + temp_z * cosf(rotation.x);

        // Rotate around Y-axis
        float temp_x = x;
        temp_z = z;
        x = temp_x * cosf(rotation.y) + temp_z * sinf(rotation.y);
        z = -temp_x * sinf(rotation.y) + temp_z * cosf(rotation.y);

        // Rotate around Z-axis
        temp_x = x;
        temp_y = y;
        x = temp_x * cosf(rotation.z) - temp_y * sinf(rotation.z);
        y = temp_x * sinf(rotation.z) + temp_y * cosf(rotation.z);

        sfa->data[i] = x;
        sfa->data[i + 1] = y;
        sfa->data[i + 2] = z;
    }
}

void sfa_scale(SFA *sfa, Vec3 scale)
{
    for (int i = 0; i < sfa->length; i += 3)
    {
        sfa->data[i] *= scale.x;
        sfa->data[i + 1] *= scale.y;
        sfa->data[i + 2] *= scale.z;
    }
}

void sfa_translate(SFA *sfa, Vec3 translation)
{
    for (int i = 0; i < sfa->length; i += 3)
    {
        sfa->data[i] += translation.x;
        sfa->data[i + 1] += translation.y;
        sfa->data[i + 2] += translation.z;
    }
}

// Orthographic projection onto the XY-plane
SFA *sfa_orthographic_project(const SFA *sfa)
{
    // new sfa will only be xyxy, but passed in one was xyzxyz
    SFA *projected = sfa_new(sfa->length / 3 * 2);
    if (!projected)
    {
        return NULL;
    }

    for (int i = 0, j = 0; i < sfa->length; i += 3, j += 2)
    {
        projected->data[j] = sfa->data[i];
        projected->data[j + 1] = sfa->data[i + 1];
    }

    return projected;
}