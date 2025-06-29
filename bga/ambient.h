#pragma once

#include "adat.h"
#include "point.h"
#include "variable.h"

enum ambientf : unsigned char {
	AmbientEnabled, AmbientNoEnvironmentEffect, AmbientGlobal, AmbientRandom
};

struct ambient : variableid {
	struct soundi {
		char			id[12];
	};
	point				position;
	unsigned short		radius;
	unsigned char		volume, volume_range; // Percent value
	unsigned short		delay, delay_range; // Delay in Seconds between sounds
	unsigned			shedule; // Hour by hour presence of sound.
	adat<soundi, 10>	sounds;
	unsigned			flags; // Seriable
	int hearing(point camera) const;
	void clear();
	bool is(ambientf v) const { return (flags & (1 << v)) != 0; }
	void update();
};
extern point hearing_center;

void initialize_area_ambients();
void update_ambients(point camera);