#pragma once

#include "arcfile.h"

struct sprite;
struct areai;

struct rfsnd : arcfile {
	void*		get();
};
struct rfvoc : rfsnd {
};
struct rfpma : arcfile {
	sprite*		get() { return (sprite*)arcfile::get(0); }
};

areai* find_area(const char* id);
rfpma* find_image(const char* id);
rfsnd* find_sound(const char* id);
rfsnd* find_voice(const char* id, int index);