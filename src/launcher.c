#include "launcher.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_PROJECTS 50
static char projects[MAX_PROJECTS][256];
static int num_projects = 0;

// This uses the Windows API to read all the folders inside a directory!
void scan_folder(const char* base_path) {
    char search_path[256];
    sprintf(search_path, "%s\\*", base_path); // Example: "example_projects\*"
    
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFileA(search_path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) return; // Folder doesn't exist
    
    do {
        // If it is a Directory (folder) and NOT a hidden system folder like "." or ".."
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                if (num_projects < MAX_PROJECTS) {
                    sprintf(projects[num_projects], "%s/%s", base_path, findFileData.cFileName);
                    num_projects++;
                }
            }
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
}

void launcher_init(void) {
    num_projects = 0;
    scan_folder("example_projects");
    scan_folder("user_projects");
}

bool launcher_update_and_render(SDL_Renderer* renderer, TTF_Font* font, char* out_path) {
    // 1. Draw a dark blue background for the launcher
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255); 
    
    // 2. Get Mouse Input
    int mouse_x, mouse_y;
    Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    bool clicked = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));

    // 3. Draw the Logo Title Text (Centered at the top)
    SDL_Color title_color = {255, 200, 50, 255};
    SDL_Surface* title_surf = TTF_RenderText_Solid(font, "-- SPRITEFORGE ENGINE --", title_color);
    if (title_surf) {
        SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
        SDL_Rect title_rect = {400 - (title_surf->w / 2), 20, title_surf->w, title_surf->h};
        SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
        SDL_DestroyTexture(title_tex);
        SDL_FreeSurface(title_surf);
    }

    // 4. Draw a button for every project we found
    int start_y = 100;
    for (int i = 0; i < num_projects; i++) {
        SDL_Rect btn = {100, start_y + (i * 60), 600, 50};
        
        // AABB Collision Detection for the mouse!
        bool hovering = (mouse_x >= btn.x && mouse_x <= btn.x + btn.w && 
                         mouse_y >= btn.y && mouse_y <= btn.y + btn.h);
                         
        if (hovering) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255); // Highlight color
            if (clicked) {
                strcpy(out_path, projects[i]);
                return true; // We clicked a project! Return TRUE!
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255); // Normal color
        }
        SDL_RenderFillRect(renderer, &btn); // Draw the box

        // 5. Render the Text inside the button!
        SDL_Color text_color = {255, 255, 255, 255};
        SDL_Surface* text_surface = TTF_RenderText_Solid(font, projects[i], text_color);
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            SDL_Rect text_rect = {btn.x + 20, btn.y + 10, text_surface->w, text_surface->h};
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
            SDL_FreeSurface(text_surface);
        }
    }
    
    return false; // No projects were clicked this frame
}
