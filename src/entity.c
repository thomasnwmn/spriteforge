#include "entity.h"
#include "render.h"
#include <string.h>

#define MAX_ENTITIES 100

static Entity entity_pool[MAX_ENTITIES];

void entity_init(void) {
    //set all entities to inactive when game starts
    for (int i = 0; i < MAX_ENTITIES; i++) {
        entity_pool[i].active = false;
    }
}

Entity* entity_create(float x, float y, float w, float h, float speed, bool is_solid, int max_frames, float anim_speed, int frame_width, float scale, SDL_Texture* tex, const char* sprite_path) {
    // find first inactive slot in array
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!entity_pool[i].active) {
            //found free spot
            entity_pool[i].active = true;
            entity_pool[i].is_solid = is_solid;
            entity_pool[i].x = x;
            entity_pool[i].y = y;
            entity_pool[i].width = w;
            entity_pool[i].height = h;
            entity_pool[i].speed = speed;
            
            // Default physics to 0 so it doesn't ruin top-down games
            entity_pool[i].vx = 0.0f;
            entity_pool[i].vy = 0.0f;
            entity_pool[i].gravity = 0.0f;
            
            entity_pool[i].texture = tex;
            if (sprite_path) {
                strncpy(entity_pool[i].sprite_path, sprite_path, sizeof(entity_pool[i].sprite_path) - 1);
                entity_pool[i].sprite_path[sizeof(entity_pool[i].sprite_path) - 1] = '\0';
            } else {
                entity_pool[i].sprite_path[0] = '\0';
            }
            
            entity_pool[i].current_frame = 0;
            entity_pool[i].max_frames = max_frames;
            entity_pool[i].frame_timer = 0.0f;
            entity_pool[i].anim_speed = anim_speed;
            entity_pool[i].frame_width = frame_width;
            entity_pool[i].scale = scale;

            return &entity_pool[i];
        }
    }

    return NULL; // no free slots
}

void entity_set_sprite(Entity* e, SDL_Texture* tex, const char* sprite_path, int max_frames, float anim_speed, int frame_width) {
    if (!e) return;
    
    // Only update if the texture actually changes, or you can just forcefully update
    // But forcing update is good to restart animations
    e->texture = tex;
    if (sprite_path) {
        strncpy(e->sprite_path, sprite_path, sizeof(e->sprite_path) - 1);
        e->sprite_path[sizeof(e->sprite_path) - 1] = '\0';
    } else {
        e->sprite_path[0] = '\0';
    }
    e->max_frames = max_frames;
    e->anim_speed = anim_speed;
    e->frame_width = frame_width;
    
    // Reset animation state
    e->current_frame = 0;
    e->frame_timer = 0.0f;
}

void entity_update_all(float delta_time) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!entity_pool[i].active) continue;

        // Apply gravity to the Y velocity
        if (entity_pool[i].gravity > 0.0f) {
            entity_pool[i].vy += entity_pool[i].gravity * delta_time;
        }

        // If the entity has velocity, move it!
        if (entity_pool[i].vx != 0.0f || entity_pool[i].vy != 0.0f) {
            entity_move(&entity_pool[i], entity_pool[i].vx * delta_time, entity_pool[i].vy * delta_time);
        }
        // ----------------------

        // if this entity has an animation
        if (entity_pool[i].max_frames > 1) {
            entity_pool[i].frame_timer += delta_time;

            // is it time to go to the next frame?
            if (entity_pool[i].frame_timer >= entity_pool[i].anim_speed) {
                entity_pool[i].frame_timer = 0.0f; // reset timer
                entity_pool[i].current_frame++;    // go to next frame

                // if we reached the end, loop back to the beginning
                if (entity_pool[i].current_frame >= entity_pool[i].max_frames) {
                    entity_pool[i].current_frame = 0;
                }
            }
        }
    }
}

void entity_render_all(SDL_Renderer* renderer, float cam_x, float cam_y) {

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_pool[i].active) {
            SDL_Rect src_rect;
            SDL_Rect* src_ptr = NULL;
            int tex_height = entity_pool[i].height; // default fallback

            if (entity_pool[i].max_frames > 1) {
                src_rect.x = entity_pool[i].current_frame * entity_pool[i].frame_width;
                src_rect.y = 0;
                src_rect.w = entity_pool[i].frame_width;
                SDL_QueryTexture(entity_pool[i].texture, NULL, NULL, NULL, &src_rect.h);
                tex_height = src_rect.h;
                src_ptr = &src_rect;
            } else {
                // If not animated, we still need to know texture dimensions for scaling
                SDL_QueryTexture(entity_pool[i].texture, NULL, NULL, &src_rect.w, &src_rect.h);
                tex_height = src_rect.h;
                entity_pool[i].frame_width = src_rect.w;
            }

            // calculate how big the image will actually be on screen
            float render_width = entity_pool[i].frame_width * entity_pool[i].scale;
            float render_height = tex_height * entity_pool[i].scale;

            // shift the image so the hitbox is perfectly in the center of it
            float offset_x = (render_width - entity_pool[i].width) / 2.0f;
            float offset_y = (render_height - entity_pool[i].height) / 2.0f;

            render_draw_texture(renderer,
                entity_pool[i].texture,
                src_ptr,
                entity_pool[i].x - offset_x,
                entity_pool[i].y - offset_y,
                render_width,
                render_height,
                cam_x,
                cam_y);
        }
    }
}

static bool check_collision(Entity* a, Entity* b) {
    return (a->x < b->x + b->width &&
            a->x + a->width > b->x &&
            a->y < b->y + b->height &&
            a->y + a->height > b->y);
}

void entity_move(Entity* e, float dx, float dy) {
    e->x += dx;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        Entity* other = &entity_pool[i];
        
        if (!other->active || !other->is_solid || other == e) continue;

        if (check_collision(e, other)) {
            e->x -= dx; // undo movement
            break;
        }
    }

    e->y += dy;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        Entity* other = &entity_pool[i];

        if (!other->active || !other->is_solid || other == e) continue;

        if (check_collision(e, other)) {
            e->y -= dy; // undo movement
            break;
        }
    }
}