#include "bsdata.h"
#include "modifier.h"
#include "script.h"

BSDATA(modifieri) = {
	{"NoModifier"},
	{"Permanent"},
	{"LayInside"},
};
assert_enum(modifieri, LayInside)