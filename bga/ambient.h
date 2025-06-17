#pragma once

#include "adat.h"
#include "point.h"

enum ambientf : unsigned char {
	AmbientEnabled, AmbientNoEnvironmentEffect, AmbientGlobal, AmbientRandom
};

struct ambient {
	typedef char resref[12];
	point			position;
	unsigned short	radius;
	unsigned char	volume, volume_range;
	unsigned short	delay, delay_range;
	unsigned		shedule;
	unsigned		flags; // Seriable
	adat<resref, 10> sounds;
	int hearing(point camera) const;
	bool is(ambientf v) const { return (flags & (1 << v)) != 0; }
};
