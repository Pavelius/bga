#pragma once

#include "arcfile.h"

struct rfsnd : arcfile {
	void*		get();
};
struct rfvoc : rfsnd {
};

rfsnd* find_sound(const char* id);
rfsnd* find_voice(const char* id, int index);