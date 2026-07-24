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

    // --- SCENE MANAGER ---
    // This tracks if we are looking at the Menu, or playing the game!
    enum EngineState { STATE_LAUNCHER, STATE_PLAYING };
    enum EngineState current_state = STATE_LAUNCHER;

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
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE
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
    
    // Automatically scale the rendering when the window resizes, maintaining the 800x600 aspect ratio!
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
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
    // borrowing the default Arial font from the Windows OS folder
    TTF_Font* ui_font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24);
    if (!ui_font) {
        printf("Could not load Arial font!\n");
    }
    
    launcher_init(); // tell the launcher to scan the directories for games
    // -------------------------------

    
    char engine_root_path[512];
    _getcwd(engine_root_path, sizeof(engine_root_path));
    Uint32 last_esc_time = 0;
    bool show_esc_message = false;

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
                    if (current_state == STATE_LAUNCHER) {
                        is_running = false;
                    } else if (current_state == STATE_PLAYING) {
                        Uint32 current_ms = SDL_GetTicks();
                        if (current_ms - last_esc_time < 2000) {
                            // Double tap!
                            current_state = STATE_LAUNCHER;
                            show_esc_message = false;
                            
                            script_cleanup();
                            asset_cleanup();
                            entity_init(); // Reset all entities
                            _chdir(engine_root_path);
                            
                            asset_init();
                            script_init(renderer); // Re-initialize Lua
                        } else {
                            last_esc_time = current_ms;
                            show_esc_message = true;
                        }
                    }
                }
            }
        }

        input_update(); // process input events

        // ----------------------------------------------------
        // THE SCENE MANAGER
        // ----------------------------------------------------
        // always clear screen at the start of the frame
        if (current_state == STATE_LAUNCHER) {
            SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        }
        SDL_RenderClear(renderer);

        if (current_state == STATE_LAUNCHER) {
            
            // drawing launcher ui
            char selected_project[256];
            if (launcher_update_and_render(renderer, ui_font, selected_project)) {
                
                // if project is clicked, select root directory, load the Lua script, and switch to PLAYING state
                _chdir(selected_project);
                
                script_load_file("game.lua");
                
                current_state = STATE_PLAYING;
            }

        } else if (current_state == STATE_PLAYING) {
            
            // update game objects here using delta_time
            entity_update_all(delta_time); 
            
            
            // tell lua to run game logic
            script_update(delta_time);

            // lua will have updated engine_camera_x and engine_camera_y by now
            entity_render_all(renderer, engine_camera_x, engine_camera_y); // render all entities
            
            if (show_esc_message) {
                if (SDL_GetTicks() - last_esc_time < 2000) {
                    render_draw_text(renderer, (struct _TTF_Font*)ui_font, "Press ESC again to return to menu", 10, 10, 255, 255, 255, 255);
                } else {
                    show_esc_message = false;
                }
            }
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