#include "arcfile.h"
#include "audio.h"
#include "bsdata.h"
#include "rfiles.h"
#include "stringbuilder.h"

extern array character_speech;

bool audio_allow_sfx = true;
bool audio_allow_music = true;

void play_music_raw(void* object);

static void* get_sound(const char* id) {
	return ((rfsnd*)arc_find(bsdata<rfsnd>::source, id))->get();
}

void play_sound(const char* id) {
	if(!id || id[0] == 0)
		return;
	if(audio_allow_sfx)
		audio_play(get_sound(id));
}

void play_sound(rfsnd* p) {
	if(!p)
		return;
	if(audio_allow_sfx)
		audio_play(p->get());
}

void play_music(const char* id) {
	if(audio_allow_music)
		play_music_raw(get_sound(id));
}

void play_music(rfsnd* p) {
	if(audio_allow_music)
		play_music_raw(p->get());
}