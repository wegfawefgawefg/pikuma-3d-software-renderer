#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>

#include "state.h"

void draw(SDL_Renderer *renderer, State *state);
void draw_cursor(SDL_Renderer *renderer, int x, int y);

#endif // DRAW_H
