#pragma once

typedef void(*fnevent)();

struct widget {
	const char*		id;
	fnevent			proc;
	explicit operator bool() const { return id != 0; }
	static void		initialize();
	void			open() const;
};
extern const widget* lastwidget;