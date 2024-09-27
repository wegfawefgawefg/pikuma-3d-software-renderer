#include <SDL2/SDL.h>

#include "input.h"

bool is_left_mouse_button_down(MouseState *mouseState)
{
    return mouseState->buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
}

bool is_right_mouse_button_down(MouseState *mouseState)
{
    return mouseState->buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

void process_input(State *state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            state->quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                state->quit = true;
            }
            break;
        }
    }
}