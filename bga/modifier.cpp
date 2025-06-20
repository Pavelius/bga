#include "bsdata.h"
#include "modifier.h"
#include "script.h"

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
};
assert_enum(modifieri, Permanent)