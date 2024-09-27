#ifndef INPUT_H
#define INPUT_H

#include "state.h"
#include "primitives.h"

typedef struct
{
    Uint32 buttons;
    IPoint2D pos;
} MouseState;

bool is_left_mouse_button_down(MouseState *mouseState);
bool is_right_mouse_button_down(MouseState *mouseState);
void process_input(State *state);

#endif // INPUT_H