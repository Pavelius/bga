#pragma once

#include "arcfile.h"

struct rfsnd : arcfile {
	void*		get();
};
struct rfvoc : rfsnd {
};
struct rfpma : arcfile {
	void*		get() { return arcfile::get(0); }
};

rfsnd* find_sound(const char* id);
rfsnd* find_voice(const char* id, int index);