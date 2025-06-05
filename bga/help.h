#pragma once

#include "array.h"
#include "nameable.h"

struct helpi : nameable {
	array&		source;
	fnvisible	filter;
};
