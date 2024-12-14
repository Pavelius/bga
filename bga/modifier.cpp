#include "bsdata.h"
#include "modifier.h"
#include "script.h"

unsigned apply_modifiers;
modifier_s modifier;

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
};
assert_enum(modifieri, Permanent)

template<> void fnscript<modifieri>(int index, int bonus) {
	modifier = (modifier_s)index;
}