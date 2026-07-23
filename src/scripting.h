#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <SDL.h>

// Initialize the Lua Virtual Machine
void script_init(SDL_Renderer* renderer);

void script_cleanup(void);

void script_load_file(const char* filename);

void script_update(float delta_time);

#endif