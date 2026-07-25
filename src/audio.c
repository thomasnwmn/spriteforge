#include "audio.h"
#include "asset_manager.h"
#include <SDL_mixer.h>
#include <stdio.h>

bool audio_init(void) {
    // Initialize SDL_mixer with MP3/OGG/FLAC support
    int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    Mix_Init(flags);

    // Open audio device
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error opening audio: %s\n", Mix_GetError());
        return false;
    }

    printf("Audio system initialized.\n");
    return true;
}

void audio_play_sfx(const char* filepath) {
    Mix_Chunk* chunk = asset_get_chunk(filepath);
    if (chunk) {
        Mix_PlayChannel(-1, chunk, 0); // -1 finds first available channel
    }
}

void audio_play_bgm(const char* filepath) {
    Mix_Music* music = asset_get_music(filepath);
    if (music) {
        Mix_PlayMusic(music, -1); // -1 means loop infinitely
    }
}

void audio_stop_bgm(void) {
    Mix_HaltMusic();
}

void audio_set_volume(float volume) {
    // Volume is from 0.0 to 1.0
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
    int mix_vol = (int)(volume * MIX_MAX_VOLUME);
    Mix_Volume(-1, mix_vol); // sets volume for all SFX channels
    Mix_VolumeMusic(mix_vol); // sets volume for BGM
}

void audio_cleanup(void) {
    Mix_CloseAudio();
    Mix_Quit();
}
