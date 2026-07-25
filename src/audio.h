#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

// Initialize the audio system
bool audio_init(void);

// Play a short sound effect
void audio_play_sfx(const char* filepath);

// Play looping background music
void audio_play_bgm(const char* filepath);

// Stop background music
void audio_stop_bgm(void);

// Set overall audio volume (0.0 to 1.0)
void audio_set_volume(float volume);

// Clean up the audio system
void audio_cleanup(void);

#endif // AUDIO_H
