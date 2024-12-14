#include "bsreq.h"
#include "bsdata.h"
#include "gender.h"

BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSDATA(genderi) = {
	{"NoGender"},
	{"Male"},
	{"Female"},
};
assert_enum(genderi, Female)