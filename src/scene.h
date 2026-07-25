#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

// Request a scene change to a new Lua file
void scene_change(const char* lua_filename);

// Check if a scene change has been requested
bool scene_is_pending(void);

// Get the pending scene filename
const char* scene_get_pending(void);

// Clear the pending scene change
void scene_clear_pending(void);

// Request to quit back to the launcher
void scene_quit_to_launcher(void);

// Check if a quit to launcher has been requested
bool scene_is_quit_pending(void);

// Clear the quit pending state
void scene_clear_quit_pending(void);

#endif // SCENE_H
