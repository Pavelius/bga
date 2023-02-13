#include "gender.h"

#pragma once

enum class_s : unsigned char;
enum race_s : unsigned char;

struct portraiti {
	const char*		id;
	unsigned char	colors[4];
	gender_s		gender;
	race_s			race;
	class_s			classv;
};
struct avatarable {
	short unsigned	portrait;
};
short unsigned		random_portrait(gender_s gender, portraiti** exclude = 0);