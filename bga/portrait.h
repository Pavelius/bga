#include "gender.h"

#pragma once

struct portraiti {
	const char*		id;
	unsigned char	colors[4];
	gender_s		gender;
	unsigned		races;
};
struct avatarable {
	short unsigned	portrait;
};
short unsigned		random_portrait(gender_s gender);