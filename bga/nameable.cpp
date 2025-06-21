#include "nameable.h"
#include "stringbuilder.h"

const char*	nameable::getname() const {
	return getnm(id);
}

const char*	nameable::getshortname() const {
	char temp[64]; stringbuilder sb(temp);
	sb.add(id);
	sb.add("Short");
	auto pn = getnme(temp);
	if(pn)
		return pn;
	return getname();
}

int compare_nameable(const void* v1, const void* v2) {
	auto p1 = *((nameable**)v1);
	auto p2 = *((nameable**)v2);
	return szcmp(p1->getname(), p2->getname());
}