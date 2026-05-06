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
	{30, 0, 1, 0},
	{1, 0, 0, 3},
	{30, 0, 2, 0},
	{30, 0, 3, 0},
	{30, 0, 4, 0},
	{1, 0, 0, 4},
	{30, 0, 5, 0},
	{30, 0, 6, 0},
	{30, 0, 7, 0},
	{1, 0, 0, 5},
	{30, 1, 8, 0}, // Year 2
	{30, 1, 9, 0},
	{1, 1, 0, 1},
	{30, 1, 10, 0},
	{30, 1, 11, 0},
	{1, 1, 0, 2},
	{30, 1, 12, 0},
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
	{30, 2, 8, 0}, // Year 3
	{30, 2, 9, 0},
	{1, 2, 0, 1},
	{30, 2, 10, 0},
	{30, 2, 11, 0},
	{1, 2, 0, 2},
	{30, 2, 12, 0},
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
	{30, 3, 8, 0}, // Year 4
	{30, 3, 9, 0},
	{1, 3, 0, 1},
	{30, 3, 10, 0},
	{30, 3, 11, 0},
	{1, 3, 0, 2},
	{30, 3, 12, 0},
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
	{1, 3, 0, 6}, // Leap year
};

static unsigned total_time_cicle() {
	auto r = 0;
	for(auto& v : time_cicle)
		r += v[0];
	return r;
}

unsigned start_year;
unsigned time_cicles_count = total_time_cicle();

static unsigned char* find_cicle(unsigned d, unsigned* rest) {
	for(auto& v : time_cicle) {
		if(d < v[0]) {
			if(rest)
				*rest = d;
			return v;
		}
		d -= v[0];
	}
	return time_cicle[0];
}

void get_game_date(unsigned minutes, int& year, int& month, int& day, int& special) {
	auto d = minutes / (24 * 60);
	auto p = find_cicle(d % time_cicles_count, &d);
	year = start_year + p[1];
	month = p[2];
	day = d;
	special = p[3];
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