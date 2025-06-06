#pragma once

#include "class.h"
#include "nameable.h"
#include "variant.h"

enum schooln : unsigned char;

struct spelli : nameable, classa {
	schooln		school;
	variants	wearing, instant;
};
struct spellv {
	char		powers[256];
};
