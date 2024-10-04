
#include "su32a.h"

#include <stdlib.h>

SU32A *su32a_new(int length)
{
    SU32A *su32a = (SU32A *)malloc(sizeof(SU32A));
    if (!su32a)
        return NULL;

    su32a->length = length;
    su32a->data = (uint32_t *)malloc(length * sizeof(uint32_t));
    if (!su32a->data)
    {
        free(su32a);
        return NULL;
    }

    return su32a;
}

void su32a_free(SU32A *su32a)
{
    if (!su32a)
        return;

    free(su32a->data);
    free(su32a);
}
