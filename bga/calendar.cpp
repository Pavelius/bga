#include "calendar.h"
#include "crt.h"

// Minimal measurment 1 is equal one round or one minute
// 1 hour is 60
// 1 day is 60 * 24

BSDATA(periodi) = {
	{"Month", 60 * 24},
	{"DayPart", 60, 24},
};
assert_enum(periodi, DayPart)