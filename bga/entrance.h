#pragma once

#include "nameable.h"
#include "point.h"

struct areai;

struct entrancei : nameable {
	areai*			area;
	point			position;
	unsigned char	orientation;
};
