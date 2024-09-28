#include <SDL2/SDL.h>

#include "input.h"
#include "globals.h"

IVec2 get_mouse_pos(void)
{
    int window_x, window_y;
    SDL_GetMouseState(&window_x, &window_y);
    int x, y;
    if (USE_GBA_BORDER)
    {
        x = (window_x - GBA_WINDOW_START_X) * RENDER_WIDTH / GBA_WINDOW_WIDTH;
        y = (window_y - GBA_WINDOW_START_Y) * RENDER_HEIGHT / GBA_WINDOW_HEIGHT;
    }
    else
    {
        x = window_x * RENDER_WIDTH / WIDTH;
        y = window_y * RENDER_HEIGHT / HEIGHT;
    }
    return (IVec2){x, y};
}

Uint32 get_mouse_buttons(void)
{
    return SDL_GetMouseState(NULL, NULL);
}

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