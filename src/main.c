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
#include <SDL2/SDL_image.h>

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
    IMG_Init(IMG_INIT_PNG);
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
    // WIDTH = dm.w * 0.8;
    // HEIGHT = dm.h * 0.8;
    WIDTH = WINDOW_WIDTH;
    HEIGHT = WINDOW_HEIGHT;
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

    // Load the gba background image
    SDL_Surface *imageSurface = IMG_Load("./assets/textures/Clear-Purple-Shell-Game-Boy-Advance.png");
    if (!imageSurface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return 1;
    }
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    // Load the pointer image
    SDL_Surface *pointerSurface = IMG_Load("./assets/textures/pointer.png");
    if (!pointerSurface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return 1;
    }
    SDL_Texture *pointerTexture = SDL_CreateTextureFromSurface(renderer, pointerSurface);
    int pointer_width = pointerSurface->w;
    int pointer_height = pointerSurface->h;
    SDL_FreeSurface(pointerSurface);

    // Load the gbalight
    SDL_Surface *gbaLightSurface = IMG_Load("./assets/textures/gbalight.png");
    if (!gbaLightSurface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return 1;
    }
    SDL_Texture *gbaLightTexture = SDL_CreateTextureFromSurface(renderer, gbaLightSurface);
    int gba_light_width = gbaLightSurface->w;
    int gba_light_height = gbaLightSurface->h;
    SDL_FreeSurface(gbaLightSurface);

    // Load the pointer image, but into a pixel buffer
    PixelBuffer *pointerBuffer = create_pixel_buffer(pointer_width, pointer_height);
    if (load_png_into_pixelbuffer("./assets/textures/pointer.png", pointerBuffer) != 0)
    {
        printf("Failed to load image into pixel buffer\n");
        return 1;
    }

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
        fade_pixel_buffer(pixel_buffer, 1);
        color_rotate(pixel_buffer, 10.0);
        draw(pixel_buffer, &state, pointerBuffer);

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

        if (USE_GBA_BORDER)
        {
            // Draw the render texture to the window
            SDL_Rect destRect = {GBA_WINDOW_START_X, GBA_WINDOW_START_Y, GBA_WINDOW_WIDTH, GBA_WINDOW_HEIGHT};
            SDL_RenderCopy(renderer, renderTexture, NULL, &destRect);

            // Draw the loaded image onto the window
            SDL_Rect imageRect = {0, 0, WIDTH, HEIGHT};
            SDL_RenderCopy(renderer, imageTexture, NULL, &imageRect);
        }
        else
        {
            // Draw the render texture to the window
            SDL_Rect destRect = {0, 0, WIDTH, HEIGHT};
            SDL_RenderCopy(renderer, renderTexture, NULL, &destRect);
        }

        // draw a red rect at mouse pos on the window
        int x, y;
        SDL_GetMouseState(&x, &y);
        // SDL_Rect rect = {x, y, 10, 10};
        // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        // SDL_RenderFillRect(renderer, &rect);

        // Draw the pointer image at the mouse
        SDL_Rect pointerRect = {x, y, pointer_width * WINDOW_SCALE, pointer_height * WINDOW_SCALE};
        SDL_RenderCopy(renderer, pointerTexture, NULL, &pointerRect);

        // Draw the gbalight image
        // vary its brightness based on sin of time
        int light_brightness = 128 + 127 * sin(SDL_GetTicks() / 1000.0 * 5.0);
        // minimum brightness of 100
        light_brightness = light_brightness < 100 ? 100 : light_brightness;
        SDL_SetTextureColorMod(gbaLightTexture, light_brightness, light_brightness, light_brightness);
        SDL_Rect gbaLightRect = {GBA_LIGHT_X, GBA_LIGHT_Y, gba_light_width * WINDOW_SCALE, gba_light_height * WINDOW_SCALE};
        SDL_RenderCopy(renderer, gbaLightTexture, NULL, &gbaLightRect);

        // FPS meter
        frameCount++;
        if (SDL_GetTicks() - fpsLastTime >= 1000) // Update FPS every second
        {
            fps = frameCount;
            frameCount = 0;
            fpsLastTime = SDL_GetTicks();
        }
        if (SHOW_FPS)
        {
            draw_fps(renderer, font, fps);
        }

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
    destroy_pixel_buffer(pixel_buffer);
    free_state(&state);

    TTF_CloseFont(font);
    SDL_DestroyTexture(renderTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}