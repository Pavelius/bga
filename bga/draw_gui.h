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
	unsigned short	frames[4];
	bool			hilited, disabled;
	static fnevent	apply;
	void			clear();
};
extern guii gui;
}
