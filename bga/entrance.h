#include "drawable.h"

#pragma once

struct entrance : drawable {
	char			name[32];
	unsigned char	orientation;
	static entrance* find(const char* id);
};
