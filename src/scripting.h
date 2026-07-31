#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <SDL.h>

void script_init(SDL_Window* window, SDL_Renderer* renderer);

void script_cleanup(void);

void script_load_file(const char* filename);

void script_update(float delta_time);

void script_push_event(const char* event_name, const char* payload);

const char* script_get_last_error(void);

void script_clear_error(void);

#endif