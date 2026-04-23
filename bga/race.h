#pragma once

#include "ability.h"
#include "colorgrad.h"
#include "variant.h"

struct rfpma;

enum classn : unsigned char;
enum racen : unsigned char {
	Human, Dwarf, Elf, Gnome, HalfElf, Halfling, HalfOrc,
};

struct racei : nameable {
	classn favor;
	colorgrad* hairs;
	colorgrad* skin;
	rfpma* resmale;
	rfpma* resfemale;
	char restype;
	variants abilities;
};