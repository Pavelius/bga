#pragma once

#include "ability.h"
#include "colorgrad.h"
#include "variant.h"

struct rfpma;

enum classn : unsigned char;
enum racen : unsigned char;

struct racei : nameable {
	classn			favor;
	colorgrad*		hairs;
	colorgrad*		skin;
	short unsigned	res[2];
	char			ws[2], ai; // Resc is mode of animation. Count of animation sets for race+gender. Can be 10, 6 or 11.
	variants		elements; // Standart initialize of npc this race
};