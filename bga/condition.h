#pragma once

#include "nameable.h"
#include "variant.h"

struct conditioni : nameable {
	typedef bool(*fntest)();
	fntest proc;
};
