#include "bsdata.h"
#include "game.h"
#include "calendar.h"

const char* months_id[] = {
	"NoMonth",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
};
const char* special_day_id[] = {
	"NoSpecialDay",
	"Highharvestide",
	"FeastOfTheMoon",
	"Midwinter",
	"Greengrass",
	"Midsummer",
	"Shieldmeet",
};
static unsigned char time_cicle[][4] = {
	// 0 - Day period,
	// 1 - Year increment
	// 2 - Month index
	// 3 - Special days index (0 if none)
	{30, 0, 8, 0}, // Year 1
	{30, 0, 9, 0},
	{1, 0, 0, 1},
	{30, 0, 10, 0},
	{30, 0, 11, 0},
	{1, 0, 0, 2},
	{30, 0, 12, 0},
	{30, 1, 1, 0},
	{1, 1, 0, 3},
	{30, 1, 2, 0},
	{30, 1, 3, 0},
	{30, 1, 4, 0},
	{1, 1, 0, 4},
	{30, 1, 5, 0},
	{30, 1, 6, 0},
	{30, 1, 7, 0},
	{1, 1, 0, 5},
	{30, 1, 8, 0}, // Year 2
	{30, 1, 9, 0},
	{1, 1, 0, 1},
	{30, 1, 10, 0},
	{30, 1, 11, 0},
	{1, 1, 0, 2},
	{30, 1, 12, 0},
	{30, 2, 1, 0},
	{1, 2, 0, 3},
	{30, 2, 2, 0},
	{30, 2, 3, 0},
	{30, 2, 4, 0},
	{1, 2, 0, 4},
	{30, 2, 5, 0},
	{30, 2, 6, 0},
	{30, 2, 7, 0},
	{1, 2, 0, 5},
	{30, 2, 8, 0}, // Year 3
	{30, 2, 9, 0},
	{1, 2, 0, 1},
	{30, 2, 10, 0},
	{30, 2, 11, 0},
	{1, 2, 0, 2},
	{30, 2, 12, 0},
	{30, 3, 1, 0},
	{1, 3, 0, 3},
	{30, 3, 2, 0},
	{30, 3, 3, 0},
	{30, 3, 4, 0},
	{1, 3, 0, 4},
	{30, 3, 5, 0},
	{30, 3, 6, 0},
	{30, 3, 7, 0},
	{1, 3, 0, 5},
	{30, 3, 8, 0}, // Year 4
	{30, 3, 9, 0},
	{1, 3, 0, 1},
	{30, 3, 10, 0},
	{30, 3, 11, 0},
	{1, 3, 0, 2},
	{30, 3, 12, 0},
	{30, 4, 1, 0},
	{1, 4, 0, 3},
	{30, 4, 2, 0},
	{30, 4, 3, 0},
	{30, 4, 4, 0},
	{1, 4, 0, 4},
	{30, 4, 5, 0},
	{30, 4, 6, 0},
	{30, 4, 7, 0},
	{1, 4, 0, 5},
	{1, 4, 0, 6}, // Leap year
};

static unsigned total_cicle_days_count() {
	auto r = 0;
	for(auto& v : time_cicle)
		r += v[0];
	return r;
}

const unsigned start_year = 1280;
unsigned cicle_days_count = total_cicle_days_count();

void get_game_date(unsigned epoch_days, int& year, int& month, int& day, int& special) {
	auto d = epoch_days;
	year = start_year + (d / cicle_days_count) * 4;
	d = d % cicle_days_count;
	for(auto& v : time_cicle) {
		if(d < v[0]) {
			year += v[1];
			month = v[2];
			special = v[3];
			day = d + 1;
			break;
		}
		d -= v[0];
	}
}

void set_game_date(unsigned& epoch_days, int year, int month, int day, int special) {
	if(year < start_year) {
		epoch_days = 0;
		return;
	}
	year -= start_year;
	unsigned d = (year / 4) * cicle_days_count;
	unsigned b = year % 4;
	if(special) {
		for(auto& v : time_cicle) {
			if(b == v[1] && special == v[3])
				break;
			d += v[0];
		}
	} else {
		// Correction of leap year cicle
		if(b == 0 && month < 8) {
			b = 4;
			d -= cicle_days_count;
		}
		for(auto& v : time_cicle) {
			if(b == v[1] && month == v[2])
				break;
			d += v[0];
		}
		d += (day - 1);
	}
	epoch_days = d;
}

int gethour() {
	return (getminute() / 60) % 24;
}

int getday() {
	return getminute() / (24 * 60);
}

bool active_time(unsigned shedule) {
	if(!shedule)
		return true;
	auto h = gethour();
	return (shedule & (1 << h)) != 0;
}