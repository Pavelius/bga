#pragma once

typedef void (*fnevent)();
struct sprite;

namespace draw {
struct guii {
	const char*		id;
	int				value;
	void*			source;
	const sprite*	res;
	unsigned short	frames[4];
	bool			hilited, disabled;
	void			clear();
};
extern guii gui;
}
