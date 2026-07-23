#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "input.h"
#include "render.h"
#include "asset_manager.h"
#include "entity.h"
#include "scripting.h"
#include "launcher.h"
#include <SDL_ttf.h>
#include <direct.h> // For _chdir on Windows

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT 100

int main(int argc, char* argv[]) {

    // --- SCENE MANAGER (STATE MACHINE) ---
    // This tracks if we are looking at the Menu, or playing the game!
    enum EngineState { STATE_LAUNCHER, STATE_PLAYING };
    enum EngineState current_state = STATE_LAUNCHER;

    // We stay in the root folder so the Launcher can see 'example_projects'!

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
    script_init(renderer);        // Start the Lua Virtual Machine
    
    // --- TTF FONT INITIALIZATION ---
    if (TTF_Init() == -1) {
        printf("Error initializing TTF: %s\n", TTF_GetError());
        return -1;
    }
    // We are borrowing the default Arial font from the Windows OS folder!
    TTF_Font* ui_font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
    if (!ui_font) {
        printf("Could not load Arial font!\n");
    }
    
    launcher_init(); // Tell the launcher to scan the directories for games!
    // -------------------------------

    // Camera variables are now in render.h and controlled by Lua!

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

        input_update(); // Process keyboard state

        // ----------------------------------------------------
        // THE SCENE MANAGER
        // ----------------------------------------------------
        // ALWAYS clear the screen at the start of the frame!
        if (current_state == STATE_LAUNCHER) {
            SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        }
        SDL_RenderClear(renderer);

        if (current_state == STATE_LAUNCHER) {
            
            // Draw the Launcher UI and check if a project was clicked!
            char selected_project[256];
            if (launcher_update_and_render(renderer, ui_font, selected_project)) {
                
                // IF WE CLICKED A PROJECT:
                // 1. Change the root directory to that project
                _chdir(selected_project);
                
                // 2. Load the game's Lua script!
                script_load_file("game.lua");
                
                // 3. Switch the engine state to PLAYING!
                current_state = STATE_PLAYING;
            }

        } else if (current_state == STATE_PLAYING) {
            
            // update game objects here using delta_time
            entity_update_all(delta_time); 
            
            
            // TELL LUA TO RUN THE GAME LOGIC!
            script_update(delta_time);

            // Lua will have updated engine_camera_x and engine_camera_y by now!
            entity_render_all(renderer, engine_camera_x, engine_camera_y); // render all entities
        }
        
        SDL_RenderPresent(renderer); // swap buffers
    }

    SDL_DestroyRenderer(renderer);
    script_cleanup(); // Clean up Lua VM
    asset_cleanup(); // Clean up loaded textures
    
    if (ui_font) TTF_CloseFont(ui_font);
    TTF_Quit(); // Clean up Font System
    
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}