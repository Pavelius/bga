#pragma once

#include "coloration.h"
#include "class.h"
#include "nameable.h"

enum alignmentn : unsigned char;
enum gendern : unsigned char;
enum dietyn : unsigned char;
enum resn : unsigned short;

struct npci : nameable, classa, coloration {
	gendern		gender;
	alignmentn	alignment;
	dietyn		diety;
	resn		res[4]; // Resources with overlays
};
