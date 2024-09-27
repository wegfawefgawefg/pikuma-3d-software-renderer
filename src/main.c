#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state.h"
#include "input.h"
#include "step.h"
#include "draw.h"

int WIDTH;
int HEIGHT;

float rand_range(float min, float max)
{
    // Generate a random float between 0 and 1
    float scale = rand() / (float)RAND_MAX;

    // Scale and shift it to the desired range
    return min + scale * (max - min);
}

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
    struct State state = {.quit = false};
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    while (!state.quit)
    {
        process_input(&state);

        step(&state);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw(renderer, &state);

        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
