#include "audio.h"
#include "manager.h"

static array resources(sizeof(manageri));

void initialize_audio() {
	manager_initialize(resources, "wav", "*.wav", true);
}

void play_sound(const char* id, short unsigned volume, fnaudiocb proc, void* object) {
	auto p = manager_get(resources, id, "wav");
	if(p)
		audio_play(p, volume, proc, object);
}

void audio_repeat(void* object, void* callback_object) {
	audio_play(object, 0xFFFF, audio_repeat, callback_object);
}