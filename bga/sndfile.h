#pragma once

#include "arcfile.h"

struct sndfile : arcfile {
	void*		get();
};
sndfile* find_sound(const char* id);
