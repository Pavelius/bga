#include "point.h"

#pragma once

namespace draw {
struct drawable : point {
	enum {
		AbsolutePosition, AutoClear
	};
	unsigned char	alpha, priority, param, flags;
	point			getscreen() const;
	bool			is(int v) const { return (flags & (1 << v)) != 0; }
	void			move(point goal, int speed, int correct = 0);
	void			set(point v) { x = v.x; y = v.y; }
	void			set(int v) { flags |= (1 << v); }
};
struct draworder : drawable {
	struct object*	parent;
	drawable		start;
	draworder*		depend;
	unsigned long	tick_start, tick_stop;
	explicit operator bool() const { return parent != 0; }
	void			clear();
	void			setduration(int v) { tick_stop = tick_start + v; }
	void			update();
	void			wait();
};
struct object : drawable {
	typedef void(*fnpaint)(const object* pointer);
	const void*		data;
	unsigned short	frame, cicle;
	static object	def;
	static fnpaint	painting;
	draworder*		addorder(int milliseconds = 1000, draworder* depend = 0);
	void			clear();
	static void		initialize();
	void			paint() const;
};
bool cameravisible(point goal, int border = 48);
void clearobjects();
void focusing(point goal);
void paintobjects();
void setcamera(point v);
void slidecamera(point v, int step = 16);
void splashscreen(unsigned milliseconds);
void showobjects();
void waitall();
object* addobject(int x, int y);
object* findobject(const void* p);
void* chooseobject();
}