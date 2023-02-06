#pragma once

typedef void (*fnevent)();
typedef bool (*fnallowrun)();

namespace draw {
struct command {
	const char*		id;
	fnevent			proc;
	unsigned		key;
	fnallowrun		allow;
	static void		execute(const char* id, int param);
};
}