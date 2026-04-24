#include "resid.h"

#pragma once

struct resinfo {
	sprite*			res;
	unsigned short	cicle;
	void			clear() { res = 0; cicle = 0; }
	void			set(const char* id, unsigned short cicle) { res = gres(id); this->cicle = cicle; }
	void			set(sprite* p1, unsigned short p2) { res = p1; cicle = p2; }
};
extern resinfo cursor;