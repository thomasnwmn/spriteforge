#include "events.h"
#include "scripting.h"
#include <string.h>

void events_init(void) {
    // Basic init if needed
}

void events_publish(const char* event_name, const char* payload) {
    if (event_name) {
        script_push_event(event_name, payload);
    }
}

void events_cleanup(void) {
}
