#include "calendar.h"
#include "crt.h"
#include "stringbuilder.h"
#include "timer.h"

// Minimal measurment 1 is equal one round or one minute
// 1 hour is 60
// 1 day is 60 * 24

BSDATA(periodi) = {
	{"Month", 60 * 24},
	{"DayPart", 60, 24},
};
assert_enum(periodi, DayPart)

int gethour() {
	// In game you have 1 second = 1 minute. So 60 seconds is hour.
	return (current_game_tick / (1000 * 60)) % 24;
}

int gethour(unsigned value) {
	return (value / 60) % 24;
}

int getday() {
	return current_game_tick / (1000 * 60 * 24);
}

int getday(unsigned value) {
	return value / (24 * 60);
}

void getpassedtime(stringbuilder& sb, unsigned value) {
	auto h = gethour(value);
	auto d = getday(value);
	if(d)
		sb.add("%1i %2", d, (d == 1) ? "day" : "days");
	if(h)
		sb.add("%1i %2", h, (h == 1) ? "hour" : "hours");
}