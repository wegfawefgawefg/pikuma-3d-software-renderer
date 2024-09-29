#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>

#include "state.h"
#include "draw_lib.h"
#include "pixel_buffer.h"
#include "assets.h"

void draw(PixelBuffer *pb, State *state, Assets *assets);
#endif // DRAW_H
