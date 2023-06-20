#include "area.h"
#include "objectid.h"

const char* objectid::getarea() const {
	return bsdata<areai>::elements[area].name;
}