#pragma once

#include "nameable.h"

enum period_s : unsigned char {
	Month, DayPart,
};

struct calendari : nameable {
	period_s		period;
	unsigned		duration;
};
struct periodi : nameable {
	unsigned		measure, range;
};

int getday();
int getday(unsigned passed);
int gethour();
int gethour(unsigned passed);
