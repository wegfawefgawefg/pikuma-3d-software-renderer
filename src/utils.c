#include "primitives.h"
#include "globals.h"

IPoint2D get_mouse_pos(void)
{
    int window_x, window_y;
    SDL_GetMouseState(&window_x, &window_y);
    int x = window_x * RENDER_WIDTH / WIDTH;
    int y = window_y * RENDER_HEIGHT / HEIGHT;
    return (IPoint2D){x, y};
}

Uint32 get_mouse_buttons(void)
{
    return SDL_GetMouseState(NULL, NULL);
}

float rand_range(float min, float max)
{
    // Generate a random float between 0 and 1
    float scale = rand() / (float)RAND_MAX;
    // Scale and shift it to the desired range
    return min + scale * (max - min);
}