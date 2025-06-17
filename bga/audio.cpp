#include "audio.h"
#include "manager.h"

static array resources(sizeof(manageri));

void initialize_audio() {
	manager_initialize(resources, "wav", "*.wav", true);
}

void play_sound(const char* id) {
	auto p = manager_get(resources, id, "wav");
	if(p)
		audio_play_memory(-1, p, 0, 0);
}

static void reapeat_sound(int channel) {
	auto p = audio_get_object(channel);
	audio_play_memory(channel, p, reapeat_sound, p);
}

void play_sound_repeated(const char* id) {
	auto p = manager_get(resources, id, "wav");
	if(p)
		audio_play_memory(-1, p, reapeat_sound, p);
}