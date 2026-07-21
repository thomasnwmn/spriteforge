#include "asset_manager.h"
#include <string.h>
#include <stdio.h>

#define MAX_TEXTURES 100

// pairs a file path with its loaded texture into memory so we can avoid loading the same texture multiple times
typedef struct {
    char filepath[256];
    SDL_Texture* texture;
} TextureCacheItem;

static TextureCacheItem texture_cache[MAX_TEXTURES];
static int texture_count = 0;

void asset_init(void) {
    texture_count = 0;

    // initialize the SDL_image library to loads PNGs
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Error initializing SDL_image: %s\n", IMG_GetError());
    }
}

SDL_Texture* asset_get_texture(SDL_Renderer* renderer, const char* filepath) {
    // search the cache to check if we loaded image already
    for (int i = 0; i < texture_count; i++) {
        // if strings match exactly, return the cached texture
        if (strcmp(texture_cache[i].filepath, filepath) == 0) {
            return texture_cache[i].texture;
        }
    }

    // if this is reaches, its not in the cahce, so we need to load it from the disk

    if (texture_count >= MAX_TEXTURES) {
        printf("Error: Texture cache is already full!\n");
        return NULL;
    }

    // IMG_LoadTexture turns a png into hardware accelerated texture that can be drawn to the screen
    SDL_Texture* new_texture = IMG_LoadTexture(renderer, filepath);
    if (!new_texture) {
        printf("Failed to load texture %s: %s\n", filepath, IMG_GetError());
        return NULL;
    }

    // save to cache for next time texture is loaded
    strncpy(texture_cache[texture_count].filepath, filepath, 256);
    // save the pointer
    texture_cache[texture_count].texture = new_texture;
    
    texture_count++;

    printf("Loaded new texture from disk: %s\n", filepath);
    return new_texture;
}

void asset_cleanup(void) {
    // loop through all cached textures and destroy them to not leak memory
    for (int i = 0; i < texture_count; i++) {
        SDL_DestroyTexture(texture_cache[i].texture);
    }
    texture_count = 0;
    IMG_Quit(); // cleanup SDL_image
}