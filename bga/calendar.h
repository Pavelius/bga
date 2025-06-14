#pragma once

#include "nameable.h"

enum periodn : unsigned char {
	Month, DayPart,
};

struct calendari : nameable {
	periodn	period;
	unsigned duration;
};
struct periodi : nameable {
	unsigned measure, range;
};

int getday();
int getday(unsigned passed);
int getminute();
int getminute(unsigned value);
int gethour();
int gethour(unsigned passed);
