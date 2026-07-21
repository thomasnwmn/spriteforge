#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL.h>

typedef enum {
    ACTION_JUMP,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_RIGHT,
    ACTION_SHOOT,
    MAX_ACTIONS
} GameAction;

// setup default keyboard shortcuts
void input_set_default_keybinds(void);

bool input_get_action(GameAction action); // Check if an action is currently being held down

// Check if an action was pressed this frame
bool input_get_action_down(GameAction action);

// Initialize the input system
void input_init(void);

// called once per frame to update keyboard states
void input_update(void);

// Check if a key is currently being held down
bool input_get_key(SDL_Scancode key);

// Check if a key was pressed exactly this frame
bool input_get_key_down(SDL_Scancode key);

#endif