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
int gethour();
int getminute();

extern const char* months_id[];
extern const char* special_day_id[];

bool active_time(unsigned shedule);

void get_game_date(unsigned epoch_days, int& year, int& month, int& day, int& special);
void set_game_date(unsigned& epoch_days, int year, int month, int day, int special = 0);
