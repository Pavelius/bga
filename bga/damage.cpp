#include "bsdata.h"
#include "damage.h"

BSDATA(damagei) = {
	{"Bludgeon"},
	{"Slashing"},
	{"Pierce"},
	{"Acid"},
	{"Cold"},
	{"Fire"},
	{"Sonic"},
};
assert_enum(damagei, Sonic)