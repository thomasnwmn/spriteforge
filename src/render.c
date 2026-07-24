#include "render.h"
#include <stdio.h>
#include <SDL_ttf.h>

float engine_camera_x = 0.0f;
float engine_camera_y = 0.0f;

void render_draw_rect(SDL_Renderer* renderer, float x, float y, float width, float height, float cam_x, float cam_y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    // First, create a rectangle struct using our float coordinates (we cast to an int since SDL renders full pixels)

    SDL_Rect rect;

    rect.x = (int)(x - cam_x);
    rect.y = (int)(y - cam_y);
    rect.w = (int)width;
    rect.h = (int)height;

    // Set draw colour for rectangle
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    // draw the rectangle to the back buffer
    SDL_RenderFillRect(renderer, &rect);

}

void render_draw_texture(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* src_rect, float x, float y, float width, float height, float cam_x, float cam_y) {
    if (!texture) return; 

    SDL_Rect dest_rect;
    dest_rect.x = (int)(x - cam_x);
    dest_rect.y = (int)(y - cam_y);
    dest_rect.w = (int)width;
    dest_rect.h = (int)height;

    SDL_RenderCopy(renderer, texture, src_rect, &dest_rect);
}

void render_draw_text(SDL_Renderer* renderer, struct _TTF_Font* font, const char* text, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!font || !text || text[0] == '\0') return;

    SDL_Color color = {r, g, b, a};
    SDL_Surface* surface = TTF_RenderText_Solid((TTF_Font*)font, text, color);
    if (!surface) {
        printf("Failed to render text surface: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create text texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest_rect;
    dest_rect.x = (int)x;
    dest_rect.y = (int)y;
    dest_rect.w = surface->w;
    dest_rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}