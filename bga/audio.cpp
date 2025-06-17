#include "audio.h"
#include "manager.h"

static array resources(sizeof(manageri));

void initialize_audio() {
	manager_initialize(resources, "wav", "*.wav", true);
}

void play_sound(const char* id) {
	auto p = manager_get(resources, id, "wav");
	if(p)
		audio_play_memory(p);
}