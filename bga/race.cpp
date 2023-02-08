#include "crt.h"
#include "race.h"

BSDATA(racei) = {
	{"Human"},
	{"Dwarf"},
	{"Elf"},
	{"HalfElf"},
	{"HalfOrc"},
};
assert_enum(racei, HalfOrc)