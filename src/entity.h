#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <SDL.h>

typedef struct {
    bool active; // is the entity active in the game world
    float x; // x position in the world
    float y; // y position in the world
    float width, height; // size of the entity
    float speed; // movement speed of the entity
    SDL_Texture* texture; // texture to render for this entity
} Entity;

// initialize the entity system
void entity_init(void);

// ask the engine for a blank entity
Entity* entity_create(float x, float y, float w, float h, float speed, SDL_Texture* tex);

// update all active entities
void entity_update_all(float delta_time);

// draw all active entities
void entity_render_all(SDL_Renderer* renderer, float cam_x, float cam_y);

#endif