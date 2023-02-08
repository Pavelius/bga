#include "ability.h"
#include "colorgrad.h"
#include "nameable.h"

#pragma once

enum race_s : unsigned char {
	Human, Dwarf, Elf, HalfElf, HalfOrc,
};

struct racei : nameable {
	colorgrad*	hairs;
	colorgrad*	skin;
};