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
	static fnevent	prepare, opening, closing;
	bool			iswindowed() const;
	static void		nextscene(const char* id);
	static long		open(const char* id);
	void			paint() const;
	static void		read(const char* url);
};
extern form* last_form;
}
