#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "state.h"
#include "input.h"
#include "step.h"
#include "draw.h"
#include "pixel_buffer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int WIDTH;
int HEIGHT;

// draw fps
void draw_fps(SDL_Renderer *renderer, TTF_Font *font, int fps)
{
    char fpsText[16];
    snprintf(fpsText, sizeof(fpsText), "FPS: %d", fps);
    SDL_Color textColor = {255, 255, 255, 255}; // White color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, fpsText, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {10, 10, textWidth, textHeight}; // Position in top-left corner
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    TTF_Init(); // Initialize SDL_ttf
    TTF_Font *font = TTF_OpenFont("./assets/fonts/DejaVuSans.ttf", 24);
    if (!font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // get monitor dims
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    // set width and height to 80% of monitor dims
    WIDTH = dm.w * 0.2;
    HEIGHT = dm.h * 0.2;
    SDL_Window *window = SDL_CreateWindow(
        "Software Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);

    // hide mouse
    SDL_ShowCursor(SDL_DISABLE);

    // Main loop
    State state = *new_state();
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create render texture
    SDL_Texture *renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
    PixelBuffer *pixel_buffer = create_pixel_buffer(RENDER_WIDTH, RENDER_HEIGHT);

    int fps = 0;
    int frameCount = 0;
    Uint32 fpsLastTime = SDL_GetTicks();
    Uint32 frameStart;
    float frameTime;

    while (!state.quit)
    {
        frameStart = SDL_GetTicks();

        process_input(&state);
        step(&state);
        // clear_pixel_buffer(pixel_buffer, 0x00000000);
        fade_pixel_buffer(pixel_buffer, 8);
        draw(pixel_buffer, &state);

        // clear the render texture
        SDL_SetRenderTarget(renderer, renderTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // clear window
        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // copy pixel buffer to render texture
        copy_to_texture(pixel_buffer, renderTexture);

        // Draw the render texture to the window
        SDL_Rect destRect = {0, 0, WIDTH, HEIGHT};
        SDL_RenderCopy(renderer, renderTexture, NULL, &destRect);

        // FPS meter
        frameCount++;
        if (SDL_GetTicks() - fpsLastTime >= 1000) // Update FPS every second
        {
            fps = frameCount;
            frameCount = 0;
            fpsLastTime = SDL_GetTicks();
        }
        draw_fps(renderer, font, fps);

        SDL_RenderPresent(renderer);

        // Frame rate limiting
        if (FRAME_LIMITING)
        {
            frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < TARGET_FRAME_TIME)
            {
                Uint32 leftover_time = TARGET_FRAME_TIME - frameTime;
                SDL_Delay(leftover_time);
            }
        }
    }

    // Clean up
    TTF_CloseFont(font);
    SDL_DestroyTexture(renderTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}