#include "gender.h"
#include "stringbuilder.h"

#pragma once

struct actable {
	gendern		gender;
	short unsigned	name;
	static void		actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gendern gender);
	void			act(const char* format, ...);
	void			say(const char* format, ...);
	const char*		getname() const { return "Pavelius"; }
};
