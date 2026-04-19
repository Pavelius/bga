#pragma once

#include "arcfile.h"

struct sndfile : arcfile {
	void*		get();
};

sndfile* find_sound(const char* id);
sndfile* find_character_sound(const char* id, int index);

int character_speech_index(sndfile* p);
int character_speech_index(const char* id);

const char* character_speech_id(short unsigned i);