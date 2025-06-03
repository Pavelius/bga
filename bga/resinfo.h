#include "resid.h"

#pragma once

struct resinfo {
	resn			id;
	unsigned short	cicle;
	void			set(resn id, unsigned short cicle) { this->id = id; this->cicle = cicle; }
};
extern resinfo cursor;