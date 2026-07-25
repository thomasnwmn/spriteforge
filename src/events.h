#ifndef EVENTS_H
#define EVENTS_H

// Initialize event system
void events_init(void);

// Publish an event by name with an optional string payload
void events_publish(const char* event_name, const char* payload);

// Clear all C-side event systems if necessary
void events_cleanup(void);

#endif
