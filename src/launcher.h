#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

// Scans the project folders for games
void launcher_init(void);

// Draws the UI. Returns true if the user clicks a project!
bool launcher_update_and_render(SDL_Renderer* renderer, TTF_Font* font, char* out_path);

#endif
