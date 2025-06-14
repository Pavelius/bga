#include "bsdata.h"
#include "game.h"
#include "calendar.h"

// Minimal measurment 1 is equal one round or one minute
// 1 hour is 60
// 1 day is 60 * 24
BSDATA(periodi) = {
	{"Month", 60 * 24},
	{"DayPart", 60, 24},
};
assert_enum(periodi, DayPart)

int gethour(unsigned value) {
	return (value / (60 * 10)) % 24;
}

int getminute(unsigned value) {
	return (value / (10)) % 60;
}

int getminute() {
	return getminute(game.get(Rounds));
}

int getday(unsigned value) {
	return value / (24 * 60 * 10);
}

int gethour() {
	return gethour(game.get(Rounds));
}

int getday() {
	return getday(game.get(Rounds));
}