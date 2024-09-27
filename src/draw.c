#include "draw.h"
#include "primitives.h"
#include "utils.h"

void draw_cursor(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x - 10, y, x + 10, y);
    SDL_RenderDrawLine(renderer, x, y - 10, x, y + 10);
}

void draw(SDL_Renderer *renderer, State *state)
{
    IPoint2D mousePos = get_mouse_pos();
    // print mouse pos
    printf("Mouse pos: %d, %d\n", mousePos.x, mousePos.y);
    draw_cursor(renderer, mousePos.x, mousePos.y);
}