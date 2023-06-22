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
	const char*		hotkey;
	unsigned		key; // Parsed value from hotkey
};
struct form {
	const char*		id;
	sliceu<control>	controls;
	static fnevent	prepare, opening, closing;
	static void		nextscene(const char* id);
	void			nextscene() const;
	long			open(bool modal) const;
	static long		open(const char* id, bool modal);
	void			paint() const;
	static void		read(const char* url);
};
extern const form* last_form;
}
