#include "bsdata.h"
#include "school.h"

BSDATA(schooli) = {
	{"Universal"},
	{"Abjuration"},
	{"Conjuration"}, // 2
	{"Divination"},
	{"Enchantment"}, // 4
	{"Evocation"}, // 6
	{"Illusion"},
	{"Necromancy"}, // 7
	{"Transmutation"},
};
assert_enum(schooli, Transmutation)