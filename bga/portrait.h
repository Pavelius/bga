#include "gender.h"

#pragma once

enum classn : unsigned char;
enum racen : unsigned char;

struct portraiti {
	const char*		id;
	unsigned char	colors[4];
	gendern			gender;
	racen			race;
	classn			classv;
};
struct avatarable {
	short unsigned	portrait;
};
short unsigned		random_portrait(gendern gender, portraiti** exclude = 0);