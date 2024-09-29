#include "colors.h"
#include <stdint.h>

uint32_t from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}
uint32_t from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

uint32_t set_alpha(uint32_t color, uint8_t alpha)
{
    return (color & 0x00FFFFFF) | (alpha << 24);
}

uint32_t color_fmul(uint32_t color, float scalar)
{
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;

    r = (uint8_t)(r * scalar);
    g = (uint8_t)(g * scalar);
    b = (uint8_t)(b * scalar);
    a = (uint8_t)(a * scalar);

    return (r << 24) | (g << 16) | (b << 8) | a;
}

// respect clamping
uint32_t add_colors(uint32_t color_a, uint32_t color_b)
{
    uint8_t r_a = (color_a >> 24) & 0xFF;
    uint8_t g_a = (color_a >> 16) & 0xFF;
    uint8_t b_a = (color_a >> 8) & 0xFF;
    uint8_t a_a = color_a & 0xFF;

    uint8_t r_b = (color_b >> 24) & 0xFF;
    uint8_t g_b = (color_b >> 16) & 0xFF;
    uint8_t b_b = (color_b >> 8) & 0xFF;
    uint8_t a_b = color_b & 0xFF;

    uint16_t r = r_a + r_b;
    uint16_t g = g_a + g_b;
    uint16_t b = b_a + b_b;
    uint16_t a = a_a + a_b;

    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;
    if (a > 255)
        a = 255;

    return (r << 24) | (g << 16) | (b << 8) | a;
}

// blend colors
uint32_t blend_colors(uint32_t color_a, uint32_t color_b)
{
    uint8_t r_a = (color_a >> 24) & 0xFF;
    uint8_t g_a = (color_a >> 16) & 0xFF;
    uint8_t b_a = (color_a >> 8) & 0xFF;
    uint8_t a_a = color_a & 0xFF;

    uint8_t r_b = (color_b >> 24) & 0xFF;
    uint8_t g_b = (color_b >> 16) & 0xFF;
    uint8_t b_b = (color_b >> 8) & 0xFF;
    uint8_t a_b = color_b & 0xFF;

    uint16_t r = (uint16_t)r_a * a_a + (uint16_t)r_b * a_b;
    uint16_t g = (uint16_t)g_a * a_a + (uint16_t)g_b * a_b;
    uint16_t b = (uint16_t)b_a * a_a + (uint16_t)b_b * a_b;
    uint16_t a = a_a + a_b * (255 - a_a) / 255;

    r = (r + 127) / 255;
    g = (g + 127) / 255;
    b = (b + 127) / 255;

    return (r << 24) | (g << 16) | (b << 8) | a;
}