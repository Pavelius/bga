#include "crt.h"
#include "color.h"
#include "point.h"
#include "resid.h"
#include "widget.h"
#include "variant.h"

#pragma once

namespace draw {
struct control : point {
	const char*		id;
	short			width, height;
	const widget*	visual;
	residi*			resource;
	unsigned short	frames[6];
	color			fore;
	int				value;
	variant			data;
};
struct form {
	const char*		id;
	sliceu<control>	controls;
	static fnevent	prepare;
	static long		choose(const char* id);
	bool			iswindowed() const;
	static void		open(const char* id);
	void			paint() const;
	static void		read(const char* url);
};
extern form* last_form;
}
