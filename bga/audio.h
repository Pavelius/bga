#pragma once

typedef void(*fnaudiocb)(void* object, void* callback_object);

void audio_play(void* object, short unsigned volume, fnaudiocb callback, void* callback_object);
void audio_reset();
void audio_update_channels();
void initialize_audio();
void music_create_player(int number_channels, int sample_rate, int bits_per_sample, fnaudiocb callback);
void music_repeat(void* object, void* callback_object);
void play_music(const char* id);
void play_music_raw(void* object);
void play_sound(const char* id, short unsigned volumne = 0xFFFF, fnaudiocb proc = 0, void* object = 0);
void play_speech(const char* id, int index);