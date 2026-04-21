#pragma once

#include "arcfile.h"

struct sprite;

struct rfard : arcfile {
	void*		get() {}
};
struct rfsnd : arcfile {
	void*		get();
};
struct rfvoc : rfsnd {
};
struct rfpma : arcfile {
	sprite*		get() { return (sprite*)arcfile::get(0); }
};

rfard* find_area(const char* id);
rfpma* find_image(const char* id);
rfsnd* find_sound(const char* id);
rfsnd* find_voice(const char* id, int index);