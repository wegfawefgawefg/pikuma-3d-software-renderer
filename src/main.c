#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state.h"
#include "input.h"

// const of the dims
const int WIDTH = 800;
const int HEIGHT = 600;

float rand_range(float min, float max)
{
    // Generate a random float between 0 and 1
    float scale = rand() / (float)RAND_MAX;

    // Scale and shift it to the desired range
    return min + scale * (max - min);
}

void draw_cursor(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x - 10, y, x + 10, y);
    SDL_RenderDrawLine(renderer, x, y - 10, x, y + 10);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
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
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // draw little rect on mouse
        // make a rect
        // SDL_Rect rect = {x, y, 10, 10};
        // SDL_Rect dog = {x, y, 10, 10};
        draw_cursor(renderer, x, y);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
