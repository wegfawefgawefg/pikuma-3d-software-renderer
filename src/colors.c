#include "colors.h"
#include <stdint.h>

uint32_t set_alpha(uint32_t color, uint8_t alpha)
{
    return (color & 0x00FFFFFF) | (alpha << 24);
}