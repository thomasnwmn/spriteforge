#include "scripting.h"
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "entity.h"
#include "asset_manager.h"
#include "input.h"
#include "render.h"

// This pointer represents our running Lua Virtual Machine!
static lua_State* L = NULL;
static SDL_Renderer* engine_renderer = NULL;

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
    Entity* new_ent = entity_create(x, y, w, h, speed, is_solid, max_frames, anim_speed, frame_w, scale, tex);
    
    // Give the memory address (pointer) back to Lua so it can save it in a variable!
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

// Lua calls this: IsActionDown(action_id)
static int l_IsActionDown(lua_State* L) {
    int action = (int)lua_tonumber(L, 1);
    bool is_down = input_get_action(action);
    
    lua_pushboolean(L, is_down);
    return 1;
}
// ----------------------------------

void script_init(SDL_Renderer* renderer) {
    engine_renderer = renderer;

    // 1. Boot up the Virtual Machine!
    L = luaL_newstate();

    // 2. Load the standard Lua libraries
    luaL_openlibs(L);
    
    // 3. Register our Bridge Functions so Lua can use them!
    lua_register(L, "CreateEntity", l_CreateEntity);
    lua_register(L, "MoveEntity", l_MoveEntity);
    lua_register(L, "IsActionDown", l_IsActionDown);
    lua_register(L, "GetEntityPosition", l_GetEntityPosition);
    lua_register(L, "SetCameraPosition", l_SetCameraPosition);

    printf("Lua scripting engine initialized.\n");
}

void script_cleanup(void) {
    if (L) {
        lua_close(L); // close the virtual machine and free memory
        L = NULL;
    }
}

void script_load_file(const char* filename) {
    if (!L) {
        printf("Error: Lua is not initialized.\n");
        return;
    }

    if (luaL_dofile(L, filename) != LUA_OK) {
        printf("Lua Error: %s\n", lua_tostring(L, -1));
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
            printf("Lua Error: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1); // clear the error from memory
        }
    } else {
        lua_pop(L, 1); // didnt write an update function
    }
}