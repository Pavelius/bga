#include "crt.h"
#include "magic.h"

BSDATA(magici) = {
	{"Mundane"},
	{"Minor"},
	{"Medium"},
	{"Major"},
};
assert_enum(magici, Major)