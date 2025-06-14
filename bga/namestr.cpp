#include "namestr.h"
#include "stringbuilder.h"

void namestr::setname(const char* format, ...) {
	XVA_FORMAT(format);
	stringbuilder sb(name);
	sb.addv(format, format_param);
}