#include "scripting.h"
#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "entity.h"
#include "asset_manager.h"
#include "input.h"
#include "render.h"

// This pointer represents our running Lua Virtual Machine
static lua_State* L = NULL;
static SDL_Window* engine_window = NULL;
static SDL_Renderer* engine_renderer = NULL;
static char last_error_msg[512] = "";

#include "scene.h"
#include "audio.h"
#include "ui.h"
#include "events.h"

// --- THE C/LUA BRIDGE FUNCTIONS ---

// Lua calls this: CreateEntity(x, y, w, h, speed, is_solid, max_frames, anim_speed, frame_w, scale, "image.png")
static int l_CreateEntity(lua_State* L) {
    float x = (float)lua_tonumber(L, 1);
    float y = (float)lua_tonumber(L, 2);
    float w = (float)lua_tonumber(L, 3);
    float h = (float)lua_tonumber(L, 4);
    float speed = (float)lua_tonumber(L, 5);
    bool is_solid = lua_toboolean(L, 6);
    int max_frames = (int)lua_tonumber(L, 7);
    float anim_speed = (float)lua_tonumber(L, 8);
    int frame_w = (int)lua_tonumber(L, 9);
    float scale = (float)lua_tonumber(L, 10);
    const char* filepath = lua_tostring(L, 11);

    SDL_Texture* tex = asset_get_texture(engine_renderer, filepath);
    Entity* new_ent = entity_create(x, y, w, h, speed, is_solid, max_frames, anim_speed, frame_w, scale, tex, filepath);
    
    // Give the memory address (pointer) back to Lua so it can save it in a variable
    lua_pushlightuserdata(L, new_ent);
    return 1; // We return 1 value to Lua
}

// Lua calls this: MoveEntity(entity_ptr, move_x, move_y)
static int l_MoveEntity(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1); // Get the pointer from Lua
    float dx = (float)lua_tonumber(L, 2);
    float dy = (float)lua_tonumber(L, 3);
    
    if (ent) entity_move(ent, dx, dy);
    return 0;
}

static int l_GetEntityPosition(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1);
    if (ent) {
        lua_pushnumber(L, ent->x);
        lua_pushnumber(L, ent->y);
        return 2; // Returns x, y
    }
    return 0;
}

static int l_SetCameraPosition(lua_State* L) {
    engine_camera_x = (float)lua_tonumber(L, 1);
    engine_camera_y = (float)lua_tonumber(L, 2);
    return 0;
}

static int l_DrawBackground(lua_State* L) {
    const char* filepath = lua_tostring(L, 1);
    float parallax = 1.0f;
    if (lua_gettop(L) >= 2) {
        parallax = (float)lua_tonumber(L, 2);
    }
    if (filepath) {
        SDL_Texture* tex = asset_get_texture(engine_renderer, filepath);
        if (tex) {
            render_draw_background(engine_renderer, tex, engine_camera_x, engine_camera_y, parallax);
        }
    }
    return 0;
}

// Lua calls this: SetEntitySprite(entity_ptr, "image.png", max_frames, anim_speed, frame_width)
static int l_SetEntitySprite(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1); // Get the pointer from Lua
    const char* filepath = lua_tostring(L, 2);
    int max_frames = (int)lua_tonumber(L, 3);
    float anim_speed = (float)lua_tonumber(L, 4);
    int frame_w = (int)lua_tonumber(L, 5);
    
    if (ent && filepath) {
        SDL_Texture* tex = asset_get_texture(engine_renderer, filepath);
        if (tex) {
            entity_set_sprite(ent, tex, filepath, max_frames, anim_speed, frame_w);
        }
    }
    return 0;
}

// Lua calls this: GetEntitySprite(entity_ptr)
static int l_GetEntitySprite(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1);
    if (ent) {
        lua_pushstring(L, ent->sprite_path);
        return 1;
    }
    return 0;
}

// Lua calls this: IsActionDown(action_id_or_name)
static int l_IsActionDown(lua_State* L) {
    GameAction action = MAX_ACTIONS;
    if (lua_isnumber(L, 1)) {
        action = (GameAction)(int)lua_tonumber(L, 1);
    } else if (lua_isstring(L, 1)) {
        const char* str = lua_tostring(L, 1);
        if (strcmp(str, "jump") == 0) action = ACTION_JUMP;
        else if (strcmp(str, "move_left") == 0 || strcmp(str, "left") == 0) action = ACTION_MOVE_LEFT;
        else if (strcmp(str, "move_up") == 0 || strcmp(str, "up") == 0) action = ACTION_MOVE_UP;
        else if (strcmp(str, "move_down") == 0 || strcmp(str, "down") == 0) action = ACTION_MOVE_DOWN;
        else if (strcmp(str, "move_right") == 0 || strcmp(str, "right") == 0) action = ACTION_MOVE_RIGHT;
        else if (strcmp(str, "shoot") == 0) action = ACTION_SHOOT;
    }
    
    bool is_down = false;
    if (action >= 0 && action < MAX_ACTIONS) {
        is_down = input_get_action(action);
    }
    
    lua_pushboolean(L, is_down);
    return 1;
}

static int l_ChangeScene(lua_State* L) {
    const char* filepath = lua_tostring(L, 1);
    if (filepath) scene_change(filepath);
    return 0;
}

static int l_QuitToLauncher(lua_State* L) {
    scene_quit_to_launcher();
    return 0;
}

static int l_DestroyEntity(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1);
    if (ent) entity_destroy(ent);
    return 0;
}

static int l_PlaySFX(lua_State* L) {
    const char* path = lua_tostring(L, 1);
    if (path) audio_play_sfx(path);
    return 0;
}

static int l_PlayBGM(lua_State* L) {
    const char* path = lua_tostring(L, 1);
    if (path) audio_play_bgm(path);
    return 0;
}

static int l_StopBGM(lua_State* L) {
    audio_stop_bgm();
    return 0;
}

static int l_SetVolume(lua_State* L) {
    float vol = (float)lua_tonumber(L, 1);
    audio_set_volume(vol);
    return 0;
}

static int l_DrawText(lua_State* L) {
    const char* text = lua_tostring(L, 1);
    float x = (float)lua_tonumber(L, 2);
    float y = (float)lua_tonumber(L, 3);
    Uint8 r = (Uint8)lua_tonumber(L, 4);
    Uint8 g = (Uint8)lua_tonumber(L, 5);
    Uint8 b = (Uint8)lua_tonumber(L, 6);
    Uint8 a = (Uint8)lua_tonumber(L, 7);
    if (text) ui_draw_text(text, x, y, r, g, b, a);
    return 0;
}

static int l_DrawHealthBar(lua_State* L) {
    float x = (float)lua_tonumber(L, 1);
    float y = (float)lua_tonumber(L, 2);
    float w = (float)lua_tonumber(L, 3);
    float h = (float)lua_tonumber(L, 4);
    float pct = (float)lua_tonumber(L, 5);
    Uint8 r = (Uint8)lua_tonumber(L, 6);
    Uint8 g = (Uint8)lua_tonumber(L, 7);
    Uint8 b = (Uint8)lua_tonumber(L, 8);
    Uint8 a = (Uint8)lua_tonumber(L, 9);
    ui_draw_health_bar(x, y, w, h, pct, r, g, b, a);
    return 0;
}

static int l_DrawButton(lua_State* L) {
    const char* text = lua_tostring(L, 1);
    float x = (float)lua_tonumber(L, 2);
    float y = (float)lua_tonumber(L, 3);
    float w = (float)lua_tonumber(L, 4);
    float h = (float)lua_tonumber(L, 5);
    bool clicked = false;
    if (text) clicked = ui_button(text, x, y, w, h);
    lua_pushboolean(L, clicked);
    return 1;
}

static int l_PublishEvent(lua_State* L) {
    const char* name = lua_tostring(L, 1);
    const char* payload = lua_tostring(L, 2);
    if (name) script_push_event(name, payload);
    return 0;
}

static int l_SetWindowTitle(lua_State* L) {
    if (engine_window && lua_isstring(L, 1)) {
        SDL_SetWindowTitle(engine_window, lua_tostring(L, 1));
    }
    return 0;
}

static int l_SetWindowSize(lua_State* L) {
    if (engine_window && engine_renderer && lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        int w = (int)lua_tonumber(L, 1);
        int h = (int)lua_tonumber(L, 2);
        SDL_SetWindowSize(engine_window, w, h);
        SDL_RenderSetLogicalSize(engine_renderer, w, h);
        engine_screen_w = w;
        engine_screen_h = h;
    }
    return 0;
}

static int l_SetBackgroundColor(lua_State* L) {
    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        Uint8 r = (Uint8)lua_tonumber(L, 1);
        Uint8 g = (Uint8)lua_tonumber(L, 2);
        Uint8 b = (Uint8)lua_tonumber(L, 3);
        render_set_bg_color(r, g, b);
    }
    return 0;
}

static int l_LoadFont(lua_State* L) {
    if (lua_isstring(L, 1) && lua_isnumber(L, 2)) {
        const char* path = lua_tostring(L, 1);
        int size = (int)lua_tonumber(L, 2);
        TTF_Font* custom_font = TTF_OpenFont(path, size);
        if (custom_font) {
            ui_set_font(custom_font);
            lua_pushboolean(L, true);
            return 1;
        } else {
            printf("Lua failed to load font %s: %s\n", path, TTF_GetError());
        }
    }
    lua_pushboolean(L, false);
    return 1;
}

static int l_BindKey(lua_State* L) {
    GameAction action = MAX_ACTIONS;
    if (lua_isnumber(L, 1)) {
        action = (GameAction)(int)lua_tonumber(L, 1);
    } else if (lua_isstring(L, 1)) {
        const char* str = lua_tostring(L, 1);
        if (strcmp(str, "jump") == 0) action = ACTION_JUMP;
        else if (strcmp(str, "move_left") == 0 || strcmp(str, "left") == 0) action = ACTION_MOVE_LEFT;
        else if (strcmp(str, "move_up") == 0 || strcmp(str, "up") == 0) action = ACTION_MOVE_UP;
        else if (strcmp(str, "move_down") == 0 || strcmp(str, "down") == 0) action = ACTION_MOVE_DOWN;
        else if (strcmp(str, "move_right") == 0 || strcmp(str, "right") == 0) action = ACTION_MOVE_RIGHT;
        else if (strcmp(str, "shoot") == 0) action = ACTION_SHOOT;
    }
    
    SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
    if (lua_isnumber(L, 2)) {
        key = (SDL_Scancode)(int)lua_tonumber(L, 2);
    } else if (lua_isstring(L, 2)) {
        key = input_parse_key_name(lua_tostring(L, 2));
    }
    
    if (action >= 0 && action < MAX_ACTIONS && key != SDL_SCANCODE_UNKNOWN) {
        input_bind_key(action, key);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

static int l_SetEntityVelocity(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1);
    if (ent && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        float vx = (float)lua_tonumber(L, 2);
        float vy = (float)lua_tonumber(L, 3);
        entity_set_velocity(ent, vx, vy);
    }
    return 0;
}

static int l_GetEntityVelocity(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1);
    if (ent) {
        float vx = 0.0f, vy = 0.0f;
        entity_get_velocity(ent, &vx, &vy);
        lua_pushnumber(L, vx);
        lua_pushnumber(L, vy);
        return 2;
    }
    return 0;
}

static int l_SetEntityGravity(lua_State* L) {
    Entity* ent = (Entity*)lua_touserdata(L, 1);
    if (ent && lua_isnumber(L, 2)) {
        float g = (float)lua_tonumber(L, 2);
        entity_set_gravity(ent, g);
    }
    return 0;
}

// ----------------------------------

void script_init(SDL_Window* window, SDL_Renderer* renderer) {
    engine_window = window;
    engine_renderer = renderer;

    // Boot up the Virtual Machine!
    L = luaL_newstate();

    // Load the standard Lua libraries
    luaL_openlibs(L);
    
    // Register our Bridge Functions so Lua can use them!
    lua_register(L, "CreateEntity", l_CreateEntity);
    lua_register(L, "MoveEntity", l_MoveEntity);
    lua_register(L, "IsActionDown", l_IsActionDown);
    lua_register(L, "GetEntityPosition", l_GetEntityPosition);
    lua_register(L, "SetCameraPosition", l_SetCameraPosition);
    lua_register(L, "SetEntitySprite", l_SetEntitySprite);
    lua_register(L, "GetEntitySprite", l_GetEntitySprite);
    lua_register(L, "DrawBackground", l_DrawBackground);
    
    lua_register(L, "ChangeScene", l_ChangeScene);
    lua_register(L, "QuitToLauncher", l_QuitToLauncher);
    lua_register(L, "DestroyEntity", l_DestroyEntity);
    lua_register(L, "PlaySFX", l_PlaySFX);
    lua_register(L, "PlayBGM", l_PlayBGM);
    lua_register(L, "StopBGM", l_StopBGM);
    lua_register(L, "SetVolume", l_SetVolume);
    lua_register(L, "DrawText", l_DrawText);
    lua_register(L, "DrawHealthBar", l_DrawHealthBar);
    lua_register(L, "DrawButton", l_DrawButton);
    lua_register(L, "PublishEvent", l_PublishEvent);

    lua_register(L, "SetWindowTitle", l_SetWindowTitle);
    lua_register(L, "SetWindowSize", l_SetWindowSize);
    lua_register(L, "SetBackgroundColor", l_SetBackgroundColor);
    lua_register(L, "LoadFont", l_LoadFont);
    lua_register(L, "BindKey", l_BindKey);
    lua_register(L, "SetEntityVelocity", l_SetEntityVelocity);
    lua_register(L, "GetEntityVelocity", l_GetEntityVelocity);
    lua_register(L, "SetEntityGravity", l_SetEntityGravity);

    // Register global Action constants in Lua
    lua_pushinteger(L, ACTION_JUMP); lua_setglobal(L, "ACTION_JUMP");
    lua_pushinteger(L, ACTION_MOVE_LEFT); lua_setglobal(L, "ACTION_MOVE_LEFT");
    lua_pushinteger(L, ACTION_MOVE_UP); lua_setglobal(L, "ACTION_MOVE_UP");
    lua_pushinteger(L, ACTION_MOVE_DOWN); lua_setglobal(L, "ACTION_MOVE_DOWN");
    lua_pushinteger(L, ACTION_MOVE_RIGHT); lua_setglobal(L, "ACTION_MOVE_RIGHT");
    lua_pushinteger(L, ACTION_SHOOT); lua_setglobal(L, "ACTION_SHOOT");

    // Register Key constants in Lua
    lua_pushinteger(L, SDL_SCANCODE_SPACE); lua_setglobal(L, "KEY_SPACE");
    lua_pushinteger(L, SDL_SCANCODE_RETURN); lua_setglobal(L, "KEY_RETURN");
    lua_pushinteger(L, SDL_SCANCODE_ESCAPE); lua_setglobal(L, "KEY_ESCAPE");
    lua_pushinteger(L, SDL_SCANCODE_UP); lua_setglobal(L, "KEY_UP");
    lua_pushinteger(L, SDL_SCANCODE_DOWN); lua_setglobal(L, "KEY_DOWN");
    lua_pushinteger(L, SDL_SCANCODE_LEFT); lua_setglobal(L, "KEY_LEFT");
    lua_pushinteger(L, SDL_SCANCODE_RIGHT); lua_setglobal(L, "KEY_RIGHT");
    lua_pushinteger(L, SDL_SCANCODE_W); lua_setglobal(L, "KEY_W");
    lua_pushinteger(L, SDL_SCANCODE_A); lua_setglobal(L, "KEY_A");
    lua_pushinteger(L, SDL_SCANCODE_S); lua_setglobal(L, "KEY_S");
    lua_pushinteger(L, SDL_SCANCODE_D); lua_setglobal(L, "KEY_D");
    lua_pushinteger(L, SDL_SCANCODE_Z); lua_setglobal(L, "KEY_Z");
    lua_pushinteger(L, SDL_SCANCODE_X); lua_setglobal(L, "KEY_X");

    last_error_msg[0] = '\0';

    printf("Lua scripting engine initialized.\n");
}

void script_cleanup(void) {
    if (L) {
        lua_close(L); // close the virtual machine and free memory
        L = NULL;
    }
    last_error_msg[0] = '\0';
}

const char* script_get_last_error(void) {
    return last_error_msg[0] != '\0' ? last_error_msg : NULL;
}

void script_clear_error(void) {
    last_error_msg[0] = '\0';
}

void script_load_file(const char* filename) {
    if (!L) {
        printf("Error: Lua is not initialized.\n");
        return;
    }

    if (luaL_dofile(L, filename) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        printf("Lua Error: %s\n", err);
        if (err) {
            strncpy(last_error_msg, err, sizeof(last_error_msg) - 1);
            last_error_msg[sizeof(last_error_msg) - 1] = '\0';
        }
        lua_pop(L, 1); // clear the error from memory
    }
}

void script_update(float delta_time) {
    if (!L) {
        printf("Error: Lua is not initialized.\n");
        return;
    }

    lua_getglobal(L, "Update"); // get the Update function from Lua

    if (lua_isfunction(L, -1)) { // check if the function actually exists
        lua_pushnumber(L, delta_time); // push the C float delta_time as a Lua number

        if (lua_pcall(L, 1, 0 , 0) != LUA_OK) {
            const char* err = lua_tostring(L, -1);
            printf("Lua Error: %s\n", err);
            if (err) {
                strncpy(last_error_msg, err, sizeof(last_error_msg) - 1);
                last_error_msg[sizeof(last_error_msg) - 1] = '\0';
            }
            lua_pop(L, 1); // clear the error from memory
        }
    } else {
        lua_pop(L, 1); // didnt write an update function
    }
}

void script_push_event(const char* event_name, const char* payload) {
    if (!L) return;
    
    lua_getglobal(L, "HandleEvent");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, event_name);
        if (payload) {
            lua_pushstring(L, payload);
        } else {
            lua_pushnil(L);
        }
        
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            const char* err = lua_tostring(L, -1);
            printf("Lua Event Error: %s\n", err);
            if (err) {
                strncpy(last_error_msg, err, sizeof(last_error_msg) - 1);
                last_error_msg[sizeof(last_error_msg) - 1] = '\0';
            }
            lua_pop(L, 1);
        }
    } else {
        lua_pop(L, 1);
    }
}