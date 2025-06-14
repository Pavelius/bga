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

const char* get_real_month(int number);
const char* get_real_month_of(int number);
