#include "audio.h"
#include "manager.h"

static array resources(sizeof(manageri));
int last_channel;

void initialize_audio() {
	manager_initialize(resources, "wav", "*.wav", true);
}

void play_sound(const char* id, short unsigned volume, fnaudiocb proc, void* object) {
	auto p = manager_get(resources, id, "wav");
	if(p)
		audio_play_memory(-1, volume, p, proc, object);
}

void audio_repeat(int channel) {
	auto p = audio_get_object(channel);
	audio_play_memory(channel, 0xFFFF, p, audio_repeat, p);
}