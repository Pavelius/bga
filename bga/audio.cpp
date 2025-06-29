#include "audio.h"
#include "manager.h"
#include "stringbuilder.h"

static array resources(sizeof(manageri));
static array characters(sizeof(manageri));

void initialize_audio() {
	manager_initialize(resources, "wav", "*.wav", true);
	manager_initialize(characters, "sounds", "*.wav", true);
}

void play_music(const char* id) {
	if(!id)
		play_music_raw(0);
	else {
		auto p = manager_get(resources, id, "wav");
		if(p)
			play_music_raw(p);
	}
}

void play_speech(const char* id, int index) {
	char temp[32]; stringbuilder sb(temp); sb.add("%1%2.2i", id, index);
	auto p = manager_get(characters, temp, "wav");
	if(p)
		audio_play(p, 0xFFFF, 0, 0);
}

void play_sound(const char* id, short unsigned volume, fnaudiocb proc, void* object) {
	auto p = manager_get(resources, id, "wav");
	if(p)
		audio_play(p, volume, proc, object);
}

bool is_played(const char* id) {
	return audio_played(manager_get(resources, id, "wav"));
}

void music_repeat(void* object, void* callback_object) {
	play_music_raw(object);
}