#include "bsdata.h"
#include "modifier.h"
#include "script.h"

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
	{"InsideBackpack"},
	{"InsideContainer"},
	{"InsideStore"},
};
assert_enum(modifieri, InsideStore)