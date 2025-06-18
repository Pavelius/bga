#pragma once

typedef void(*fnaudiocb)(int channel);

extern int last_channel;

void audio_play_memory(int channel, short unsigned volume, void* object, fnaudiocb callback, void* callback_object);
void audio_repeat(int channel);
void audio_update_channels();
void initialize_audio();
void play_sound(const char* id, short unsigned volumne = 0xFFFF, fnaudiocb proc = 0, void* object = 0);

short unsigned audio_get_volume(int channel);

void* audio_get_object(int channel);