#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    bool is_running = true;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow(
        "SpriteForge Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        0
    );

    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1, // picks first graphics driver that matches the requested flags
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC  // hardware acceleration and lock monitor refresh rate
    );

    if (!renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    is_running = true;

    Uint64 previous_ticks = SDL_GetPerformanceCounter();
    float delta_time = 0.0f;

    while (is_running) {
        Uint64 current_ticks = SDL_GetPerformanceCounter();
        Uint64 frequency = SDL_GetPerformanceFrequency();

        delta_time = (float)(current_ticks - previous_ticks) / (float)frequency;
        previous_ticks = current_ticks;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }
            }
        }

        // update game objects here using delta_time for frame-independent movement

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);

        SDL_RenderClear(renderer);

        // draw game objects here

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}