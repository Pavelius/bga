#pragma once

struct rfsnd;

int audio_lenght(const void* object);

void audio_play(void* object);
bool audio_played(const void* object);
void audio_reset();
void audio_update_channels();
void play_music(const char* id);
void play_music(rfsnd* p);
void play_sound(const char* id);
void play_sound(rfsnd* p);