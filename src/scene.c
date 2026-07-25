#include "scene.h"
#include <string.h>

static char pending_scene_file[256] = "";
static bool has_pending_scene = false;
static bool has_quit_pending = false;

void scene_change(const char* lua_filename) {
    if (lua_filename) {
        strncpy(pending_scene_file, lua_filename, sizeof(pending_scene_file) - 1);
        pending_scene_file[sizeof(pending_scene_file) - 1] = '\0';
        has_pending_scene = true;
    }
}

bool scene_is_pending(void) {
    return has_pending_scene;
}

const char* scene_get_pending(void) {
    return pending_scene_file;
}

void scene_clear_pending(void) {
    has_pending_scene = false;
    pending_scene_file[0] = '\0';
}

void scene_quit_to_launcher(void) {
    has_quit_pending = true;
}

bool scene_is_quit_pending(void) {
    return has_quit_pending;
}

void scene_clear_quit_pending(void) {
    has_quit_pending = false;
}
