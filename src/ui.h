#ifndef UI_H
#define UI_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

void ui_init(SDL_Renderer* renderer, TTF_Font* font);

void ui_draw_text(const char* text, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void ui_draw_health_bar(float x, float y, float w, float h, float percentage, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void ui_draw_rect(float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Simple button that returns true if clicked this frame
bool ui_button(const char* text, float x, float y, float w, float h);

#endif
