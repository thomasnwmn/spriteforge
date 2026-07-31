#include "ui.h"
#include "render.h"
#include <SDL_ttf.h>

static SDL_Renderer* ui_renderer = NULL;
static TTF_Font* ui_default_font = NULL;

void ui_init(SDL_Renderer* renderer, TTF_Font* font) {
    ui_renderer = renderer;
    ui_default_font = font;
}

void ui_set_font(TTF_Font* font) {
    if (font) {
        ui_default_font = font;
    }
}

void ui_draw_text(const char* text, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!ui_renderer || !ui_default_font || !text) return;
    render_draw_text(ui_renderer, ui_default_font, text, x, y, r, g, b, a);
}

void ui_draw_rect(float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!ui_renderer) return;
    SDL_SetRenderDrawColor(ui_renderer, r, g, b, a);
    SDL_Rect rect = { (int)x, (int)y, (int)w, (int)h };
    SDL_RenderFillRect(ui_renderer, &rect);
}

void ui_draw_health_bar(float x, float y, float w, float h, float percentage, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!ui_renderer) return;
    
    // Background (gray)
    SDL_SetRenderDrawColor(ui_renderer, 100, 100, 100, a);
    SDL_Rect bg_rect = { (int)x, (int)y, (int)w, (int)h };
    SDL_RenderFillRect(ui_renderer, &bg_rect);
    
    // Foreground (colored by percentage)
    if (percentage < 0.0f) percentage = 0.0f;
    if (percentage > 1.0f) percentage = 1.0f;
    
    SDL_SetRenderDrawColor(ui_renderer, r, g, b, a);
    SDL_Rect fg_rect = { (int)x, (int)y, (int)(w * percentage), (int)h };
    SDL_RenderFillRect(ui_renderer, &fg_rect);
}

bool ui_button(const char* text, float x, float y, float w, float h) {
    if (!ui_renderer) return false;
    
    int mx, my;
    Uint32 mouse_state = SDL_GetMouseState(&mx, &my);
    
    bool is_hovered = (mx >= x && mx <= x + w && my >= y && my <= y + h);
    bool is_clicked = is_hovered && (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
    
    if (is_hovered) {
        ui_draw_rect(x, y, w, h, 150, 150, 150, 255);
    } else {
        ui_draw_rect(x, y, w, h, 100, 100, 100, 255);
    }
    
    ui_draw_text(text, x + 10, y + 10, 255, 255, 255, 255);
    
    // Only return true on click
    static bool was_down = false;
    bool clicked_this_frame = is_clicked && !was_down;
    was_down = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
    
    return clicked_this_frame;
}
