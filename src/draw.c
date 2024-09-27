#include "draw.h"

void draw_cursor(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x - 10, y, x + 10, y);
    SDL_RenderDrawLine(renderer, x, y - 10, x, y + 10);
}

void draw(SDL_Renderer *renderer, struct State *state)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    draw_cursor(renderer, x, y);
}