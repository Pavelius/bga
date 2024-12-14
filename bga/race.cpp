#include "bsdata.h"
#include "race.h"

BSDATA(racei) = {
	{"Human"},
	{"Dwarf"},
	{"Elf"},
	{"Gnome"},
	{"HalfElf"},
	{"Halfling"},	
	{"HalfOrc"},
};
assert_enum(racei, HalfOrc)