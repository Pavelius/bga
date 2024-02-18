#pragma once

#include "stringbuilder.h"

template<class T>
void ftgetinfo(const void* object, stringbuilder& sb) {
	((T*)object)->getinfo(sb);
}
