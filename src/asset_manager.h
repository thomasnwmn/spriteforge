#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <SDL.h>

#include <SDL_image.h>
#include <SDL_mixer.h>

void asset_init(void);

SDL_Texture* asset_get_texture(SDL_Renderer* renderer, const char* file_path);

Mix_Chunk* asset_get_chunk(const char* file_path);
Mix_Music* asset_get_music(const char* file_path);

void asset_cleanup(void);

#endif
