#include "color.h"
#include "drawable.h"

#pragma once

struct floattext : drawable {
	const char*		format;
	color			fore;
	rect			box;
	unsigned long	stop_visible;
	const void*		data;
	explicit operator bool() const { return format != 0; }
	void			clear();
	void			paint() const;
};
void add_float_text(point position, const char* format, int width, unsigned millisecond, const void* data);