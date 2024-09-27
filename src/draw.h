#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>

#include "state.h"
#include "draw_lib.h"
#include "pixel_buffer.h"

void draw(PixelBuffer *pb, State *state);

#endif // DRAW_H
