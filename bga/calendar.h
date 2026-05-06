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
int getminute();
int gethour();

extern const char* months_id[];
extern const char* special_day_id[];

bool active_time(unsigned shedule);

void get_game_date(unsigned minutes, int& year, int& month, int& day, int& special);
void set_game_date(unsigned& minutes, int year, int month, int day, int special = 0);
