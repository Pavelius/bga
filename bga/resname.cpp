#include "resname.h"
#include "stringbuilder.h"

bool resname::operator==(const char* v) const {
	return szcmp(name, v) == 0;
}

void resname::set(const char* p) {
	stringbuilder sb(name);
	sb.add(p);
}

const char* resname::getname() const {
	return getnm(name);
}

int compare_resname_name(const void* v1, const void* v2) {
	auto p1 = (resname*)v1;
	auto p2 = (resname*)v2;
	return szcmp(p1->getname(), p2->getname());
}