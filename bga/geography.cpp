#include "bsdata.h"
#include "geography.h"

BSDATA(geographyi) = {
	{"North"},
	{"East"},
	{"South"},
	{"West"},
};
assert_enum(geographyi, West)