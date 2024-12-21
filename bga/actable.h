#pragma once

enum gendern : unsigned char;

struct actable {
	gendern			gender;
	short unsigned	name;
	void			actv(const char* format, const char* format_param, const char* line_feed) const;
	const char*		getname() const { return "Pavelius"; }
};
