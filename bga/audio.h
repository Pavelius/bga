#pragma once

typedef void(*fnaudiocb)(void* object, void* callback_object);

void audio_play(void* object, short unsigned volume, fnaudiocb callback, void* callback_object);
void audio_repeat(void* object, void* callback_object);
void audio_update_channels();
void initialize_audio();
void play_sound(const char* id, short unsigned volumne = 0xFFFF, fnaudiocb proc = 0, void* object = 0);