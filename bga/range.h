#include "nameable.h"

#pragma once

enum rangen : unsigned char {
	Touch, Reach, You,
	Range10,
	Range20,
	Range30,
	Range50,
	Range60,
	Range70,
	Range80,
	Range100, // SightOfCaster
	Range120,
};
struct rangei : nameable {
};