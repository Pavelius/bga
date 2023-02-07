#include "resid.h"

#pragma once

struct resinfo {
	res::token		id;
	unsigned short	cicle;
	void			set(res::token id, unsigned short cicle) { this->id = id; this->cicle = cicle; }
};
extern resinfo cursor;