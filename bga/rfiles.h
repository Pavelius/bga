#pragma once

#include "arcfile.h"

struct sprite;

struct rfsnd : arcfile {
	void*		get();
};
struct rfvoc : rfsnd {
};
struct rfpma : arcfile {
	sprite*		get() { return (sprite*)arcfile::get(0); }
};

rfsnd* find_sound(const char* id);
rfsnd* find_voice(const char* id, int index);