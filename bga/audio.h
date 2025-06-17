#pragma once

typedef void(*fnaudiocb)(int channel);

void audio_play_memory(int channel, void* object, fnaudiocb callback, void* callback_object);
void audio_set_volume(int channel, short unsigned volume);
void audio_update_channels();
void initialize_audio();
void play_sound(const char* id);
void play_sound_repeated(const char* id);

short unsigned audio_get_volume(int channel);

void* audio_get_object(int channel);