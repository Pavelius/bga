#include "drawable.h"
#include "variable.h"

#pragma once

struct sprite;

struct animation : drawable, variableid {
	struct info {
		const sprite*	source;
		int				frame;
		unsigned		flags;
		unsigned char	transparent;
		explicit operator bool() const { return source != 0; }
	};
	unsigned			schedule;
	char				rsname[9], rsname_pallette[9];
	unsigned short		circle, frame, start_frame;
	unsigned			flags;
	unsigned short		height;
	unsigned char		transparency;
	unsigned char		chance_loop;
	unsigned char		skip_cycles;
	bool				isvisible() const;
	void				paint() const;
};
