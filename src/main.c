#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "state.h"
#include "input.h"
#include "step.h"
#include "draw.h"

#include <SDL2/SDL.h>

int WIDTH;
int HEIGHT;

// void test_mouse_state(SDL_Window *window)
// {
//     int x, y;
//     Uint32 buttons = SDL_GetMouseState(&x, &y);

//     int window_x, window_y;
//     SDL_GetWindowPosition(window, &window_x, &window_y);

//     printf("Mouse position: (%d, %d)\n", x, y);
//     printf("Window position: (%d, %d)\n", window_x, window_y);
//     printf("Left button: %s\n", (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) ? "pressed" : "not pressed");
//     printf("Right button: %s\n", (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? "pressed" : "not pressed");
// }

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    // get monitor dims
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    // set width and height to 80% of monitor dims
    WIDTH = dm.w * 0.8;
    HEIGHT = dm.h * 0.8;

    SDL_Window *window = SDL_CreateWindow(
        "Software Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0
        // uncomment the line below to make the window full screen
        // | SDL_WINDOW_FULLSCREEN
        // uncomment the line above to make the window full screen
    );

    // hide mouse
    SDL_ShowCursor(SDL_DISABLE);

    // Main loop
    State state = {.quit = false};
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create render texture
    SDL_Texture *renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);

    // SDL_RenderSetLogicalSize(renderer, RENDER_WIDTH, RENDER_HEIGHT); // for mouse position

    while (!state.quit)
    {
        process_input(&state);
        step(&state);

        // Set render target to our texture
        SDL_SetRenderTarget(renderer, renderTexture);

        // Clear the render texture
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw to the render texture
        draw(renderer, &state);

        // Reset render target to the window
        SDL_SetRenderTarget(renderer, NULL);

        // Clear the window
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the render texture to the window
        SDL_Rect destRect = {0, 0, WIDTH, HEIGHT};
        SDL_RenderCopy(renderer, renderTexture, NULL, &destRect);

        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyTexture(renderTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}