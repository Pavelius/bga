#include "ability.h"
#include "colorgrad.h"

#pragma once

enum racen : unsigned char {
	Human, Dwarf, Elf, Gnome, HalfElf, Halfling, HalfOrc,
};

struct racei : nameable {
	colorgrad* hairs;
	colorgrad* skin;
};