#include "input.h"
#include <string.h> // for memcpy

#define MAX_KEYBOARD_KEYS 512

static Uint8 current_keyboard_state[MAX_KEYBOARD_KEYS];
static Uint8 previous_keyboard_state[MAX_KEYBOARD_KEYS];

void input_init(void) {
    // clear both arrays to 0 when the game starts
    memset(current_keyboard_state, 0, MAX_KEYBOARD_KEYS);
    memset(previous_keyboard_state, 0, MAX_KEYBOARD_KEYS);
}

void input_update(void) {
    // Copy current state into the previous state

    memcpy(previous_keyboard_state, current_keyboard_state, MAX_KEYBOARD_KEYS);

    // Get the new current state directly from SDL
    const Uint8* state = SDL_GetKeyboardState(NULL);

    // copy the internal array into our own current_keyboard_state array
    memcpy(current_keyboard_state, state, MAX_KEYBOARD_KEYS);
}

bool input_get_key(SDL_Scancode key) {
    // return true if the key is pressed in the current frame
    return current_keyboard_state[key] == 1;
}

bool input_get_key_down(SDL_Scancode key) {
    // return true if it is pressed now, but was not pressed in previous frame
    return (current_keyboard_state[key] == 1 && previous_keyboard_state[key] == 0);
}

static SDL_Scancode keybinds[MAX_ACTIONS];

void input_set_default_keybinds(void) {
    keybinds[ACTION_JUMP] = SDL_SCANCODE_SPACE;
    keybinds[ACTION_MOVE_UP] = SDL_SCANCODE_W;
    keybinds[ACTION_MOVE_DOWN] = SDL_SCANCODE_S;
    keybinds[ACTION_MOVE_LEFT] = SDL_SCANCODE_A;
    keybinds[ACTION_MOVE_RIGHT] = SDL_SCANCODE_D;
    keybinds[ACTION_SHOOT] = SDL_SCANCODE_RETURN;
}

bool input_get_action(GameAction action) {
    if (action < 0 || action >= MAX_ACTIONS) {
        return false;
    }

    return input_get_key(keybinds[action]);
}

bool input_get_action_down(GameAction action) {
    // make sure we dont crash if invalid action is passed in
    if (action < 0 || action >= MAX_ACTIONS) {
        return false;
    }

    // look up the physical key assigned to this action
    SDL_Scancode assigned_key = keybinds[action];

    // Check if that physical key was just pressed
    return input_get_key_down(assigned_key);
}