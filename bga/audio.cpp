#include "arcfile.h"
#include "audio.h"
#include "bsdata.h"
#include "sndfile.h"

bool audio_allow_sfx = true;
bool audio_allow_music = true;

void play_music_raw(void* object);

void initialize_audio() {
	arc_open(bsdata<sndfile>::source, "art/sound.arc");
}

static void* get_sound(const char* id) {
	return ((sndfile*)arc_find(bsdata<sndfile>::source, id))->get();
}

void play_sound(const char* id) {
	if(!id || id[0] == 0)
		return;
	if(audio_allow_sfx)
		audio_play(get_sound(id));
}

void play_sound(sndfile* p) {
	if(!p)
		return;
	if(audio_allow_sfx)
		audio_play(p->get());
}

void play_music(const char* id) {
	if(audio_allow_music)
		play_music_raw(get_sound(id));
}

void play_music(sndfile* p) {
	if(audio_allow_music)
		play_music_raw(p->get());
}