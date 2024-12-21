#include "nameable.h"

#pragma once

enum rangen : unsigned char {
	Touch, Reach,
	Range10,
	Range20,
	Range30,
	Range50,
	Range60,
	Range70,
	Range80,
	Range100,
	Range120,
};
struct rangei : nameable {
};