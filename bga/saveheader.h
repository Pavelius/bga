#pragma once

#include "color.h"

struct saveheaderi {
	char	name[32];
	color	screenshoot[102][77];
	color	party[6][20][22];
	void clear();
	void create();
	bool read(const char* id);
};
struct rowsaveheaderi : saveheaderi {
	char	file[12];
};