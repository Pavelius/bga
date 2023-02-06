#pragma once

typedef void (*fnevent)();

namespace draw {
struct command {
	const char*		id;
	fnevent			proc;
	unsigned		key;
	static void		execute(const char* id);
};
}
