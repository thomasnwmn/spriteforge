#include "entity.h"
#include "render.h"

#define MAX_ENTITIES 100

static Entity entity_pool[MAX_ENTITIES];

void entity_init(void) {
    //set all entities to inactive when game starts
    for (int i = 0; i < MAX_ENTITIES; i++) {
        entity_pool[i].active = false;
    }
}

Entity* entity_create(float x, float y, float w, float h, float speed, SDL_Texture* tex) {
    // find first inactive slot in array
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!entity_pool[i].active) {
            //found free spot
            entity_pool[i].active = true;
            entity_pool[i].x = x;
            entity_pool[i].y = y;
            entity_pool[i].width = w;
            entity_pool[i].height = h;
            entity_pool[i].speed = speed;
            entity_pool[i].texture = tex;

            return &entity_pool[i];
        }
    }

    return NULL; // no free slots
}

void entity_update_all(float delta_time) {
    // for now, blank, but in the future, enemies could wander/move around on their own
}

void entity_render_all(SDL_Renderer* renderer, float cam_x, float cam_y) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity_pool[i].active) {
            render_draw_texture(renderer,
                entity_pool[i].texture,
                entity_pool[i].x,
                entity_pool[i].y,
                entity_pool[i].width,
                entity_pool[i].height,
                cam_x,
                cam_y);
        }
    }
}