#pragma once

typedef void(*fnevent)();

struct widget {
	const char*		id;
	fnevent			proc;
	explicit operator bool() const { return id != 0; }
	static void		initialize();
	static void		open(const char* id);
};
extern const widget* lastwidget;