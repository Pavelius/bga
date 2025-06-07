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