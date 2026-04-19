#include "bsreq.h"
#include "stringbuilder.h"
#include "sndfile.h"

BSDATAD(sndfile)
BSMETA(sndfile) = {
	BSREQ(id),
	{}};

array character_speech(sizeof(sndfile));

void* acm_encoder(const char* in_url, unsigned file_size, unsigned file_position, int force_channel);

static void* acm_encoder(const char* url, unsigned file_size, unsigned file_position) {
	return acm_encoder(url, file_size, file_position, 0);
}

void* sndfile::get() {
	return arcfile::get(acm_encoder);
}

sndfile* find_sound(const char* id) {
	return (sndfile*)arc_find(bsdata<sndfile>::source, id);
}

sndfile* find_character_sound(const char* id, int index) {
	char temp[12]; stringbuilder sb(temp);
	sb.add(id);
	auto n = zlen(temp);
	temp[n - 1] = '0' + (index % 10);
	temp[n - 2] = '0' + ((index / 10) % 10);
	return (sndfile*)arc_find(character_speech, temp);
}

void initialize_audio() {
	arc_open(bsdata<sndfile>::source, "art/sound.arc");
	arc_open(character_speech, "art/soundchr.arc");
}

int character_speech_index(sndfile* p) {
	return character_speech.indexof(p);
}

int character_speech_index(const char* id) {
	return character_speech_index((sndfile*)arc_find(character_speech, id));
}

const char* character_speech_id(short unsigned i) {
	return ((sndfile*)character_speech.ptr(i))->id;
}