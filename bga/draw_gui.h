#pragma once

typedef void (*fnevent)();
struct sprite;

namespace draw {
struct guii {
	const char*		id;
	const char*		text;
	int				value;
	void*			source;
	const sprite*	res;
	unsigned short	frames[6];
	bool			hilited, checked, disabled;
	void			clear();
};
extern guii gui;
}
