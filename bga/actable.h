#include "gender.h"
#include "stringbuilder.h"

#pragma once

struct actable {
	gendern			gender;
	short unsigned	name;
	void			actv(stringbuilder& sb, const char* format, const char* format_param, char separator) const;
	void			act(const char* format, ...);
	void			say(const char* format, ...);
	const char*		getname() const { return "Pavelius"; }
};
