#include "nameable.h"

#pragma once

enum alignmentn : unsigned char;

struct alignmenti : nameable {
	alignmentn		getindex() const;
};
