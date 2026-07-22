#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>

void render_draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float cam_x, float cam_y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Draw a texture to the screen!
void render_draw_texture(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* src_rect, float x, float y, float width, float height, float cam_x, float cam_y);

#endif