#include "gender.h"
#include "stringbuilder.h"

#pragma once

struct actable {
	gender_s		gender;
	short unsigned	name;
	static void		actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s gender);
	void			act(const char* format, ...);
	void			say(const char* format, ...);
};
