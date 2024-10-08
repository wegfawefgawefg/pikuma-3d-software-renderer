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

            // use arrow keys to move camera x and y
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                state->camera_pos.x -= state->scale;
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {
                state->camera_pos.x += state->scale;
            }
            if (event.key.keysym.sym == SDLK_UP)
            {
                state->camera_pos.y += state->scale;
            }
            if (event.key.keysym.sym == SDLK_DOWN)
            {
                state->camera_pos.y -= state->scale;
            }

            // scale up and down from 0.2 to 50 with mouse scroll
            float scale_speed = 1.0;
            if (event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_KP_PLUS)
            {
                state->scale += scale_speed;
            }
            if (event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_KP_MINUS)
            {
                state->scale -= scale_speed;
            }
            // clamp using min and max
            state->scale = fmax(1.0, fmin(50, state->scale));

            // print camera position

            // wasd to move pointer on x and z
            if (event.key.keysym.sym == SDLK_a)
            {
                state->pointer_pos.x -= 1;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                state->pointer_pos.x += 1;
            }
            if (event.key.keysym.sym == SDLK_w)
            {
                state->pointer_pos.y -= 1;
            }
            if (event.key.keysym.sym == SDLK_s)
            {
                state->pointer_pos.y += 1;
            }

            // clamp between 0 and 10
            state->pointer_pos.x = fmax(0, fmin(9, state->pointer_pos.x));
            state->pointer_pos.y = fmax(0, fmin(9, state->pointer_pos.y));

            // if press space, ripple center is set to pointer pos
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                state->ripple_center = state->pointer_pos;
                state->ripple_magnitude = 1.0;

                // print ripple center
                printf("ripple center: %f, %f\n", state->ripple_center.x, state->ripple_center.y);
            }

            break;
        }
    }
}