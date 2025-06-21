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
	auto p = manager_get(resources, id, "wav");
	if(p)
		play_music_raw(p);
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

void audio_repeat(void* object, void* callback_object) {
	if(!callback_object)
		return;
	audio_play(object, 0xFFFF, audio_repeat, callback_object);
}