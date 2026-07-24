#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <SDL.h>

typedef struct {
    bool active; // is the entity active in the game world
    bool is_solid; // should this object block movement
    float x; // x position in the world
    float y; // y position in the world
    float width, height; // size of the entity
    float speed;       // Movement speed
    
    // --- Physics ---
    float vx, vy;      // Velocity (Direction + Speed)
    float gravity;     // Gravity force (e.g. 9.8)
    // ---------------
    
    SDL_Texture* texture; // The image to drawender for this entity
    char sprite_path[256]; // The file path of the current sprite image

    int current_frame;
    int max_frames;
    float frame_timer;
    float anim_speed;
    int frame_width;
    float scale;
} Entity;

// initialize the entity system
void entity_init(void);

// Ask the engine to give us a new blank entity to use
Entity* entity_create(float x, float y, float hitbox_w, float hitbox_h, float speed, bool is_solid, int max_frames, float anim_speed, int frame_width, float scale, SDL_Texture* tex, const char* sprite_path);

// Change the sprite/animation of an entity
void entity_set_sprite(Entity* e, SDL_Texture* tex, const char* sprite_path, int max_frames, float anim_speed, int frame_width);

// update all active entities
void entity_update_all(float delta_time);

// draw all active entities
void entity_render_all(SDL_Renderer* renderer, float cam_x, float cam_y);

void entity_move(Entity* entity, float dx, float dy);

#endif