#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "input.h"
#include "render.h"
#include "asset_manager.h"
#include "entity.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT 100

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
    }
    is_running = true;

    Uint64 previous_ticks = SDL_GetPerformanceCounter();
    float delta_time = 0.0f;

    input_init(); // Initialize the input system
    asset_init(); // Initialize the asset manager
    entity_init(); // Initialize the entity system
    input_set_default_keybinds(); // Set default keybinds for actions


    // --- Camera Variables ---
    float camera_x = 0.0f;
    float camera_y = 0.0f;

    // loading player textures
    SDL_Texture* player_texture = asset_get_texture(renderer, "assets/Soldier_Idle.png");
    SDL_Texture* player_texture2 = asset_get_texture(renderer, "assets/player.png");

    Entity* player = entity_create(400.0f, 300.0f, 40.0f, 50.0f, 300.0f, true, 6, 0.15f, 100, 2.5f, player_texture);
    
    entity_create(100.0f, 100.0f, 100.0f, 100.0f, 0.0f, true, 1, 0.0f, 100, 0.1236f, player_texture2);
    entity_create(600.0f, 150.0f, 100.0f, 100.0f, 0.0f, true, 1, 0.0f, 100, 0.1236f, player_texture2);
    entity_create(250.0f, 500.0f, 100.0f, 100.0f, 0.0f, true, 1, 0.0f, 100, 0.1236f, player_texture2);

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

        input_update(); // Update the input system

        // update game objects here using delta_time
        entity_update_all(delta_time); // Update all active entities

        float move_x = 0.0f;
        float move_y = 0.0f;

        if (input_get_action_down(ACTION_JUMP)) printf("The player JUMPED!\n");

        if (input_get_action(ACTION_MOVE_LEFT)) move_x -= player->speed * delta_time;
        // move right
        if (input_get_action(ACTION_MOVE_RIGHT)) move_x += player->speed * delta_time;

        if (input_get_action(ACTION_MOVE_UP)) move_y -= player->speed * delta_time;

        if (input_get_action(ACTION_MOVE_DOWN)) move_y += player->speed * delta_time;

        // Ask the entity system to safely move the player!
        entity_move(player, move_x, move_y);

        camera_x = player->x - 400.0f + (player->width / 2.0f);
        camera_y = player->y - 300.0f + (player->height / 2.0f);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);

        SDL_RenderClear(renderer);

        // draw game objects here
        // represents a tree or a building in the world
        render_draw_rect(renderer, 0, 0, 100, 100, camera_x, camera_y, 0, 255, 0, 255); // Draw a green square at the top-left corner

        entity_render_all(renderer, camera_x, camera_y); // Render all active entities

        // swap the buffer
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    asset_cleanup(); // Clean up loaded textures
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}