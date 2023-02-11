#include "crt.h"
#include "modifier.h"

unsigned apply_modifiers;
modifier_s modifier;

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
};
assert_enum(modifieri, Permanent)