#include "ability.h"
#include "colorgrad.h"

#pragma once

enum race_s : unsigned char {
	Human, Dwarf, Elf, Gnome, HalfElf, Halfling, HalfOrc,
};

struct racei : nameable {
	colorgrad*	hairs;
	colorgrad*	skin;
};