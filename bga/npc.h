#pragma once

#include "ability.h"
#include "coloration.h"
#include "class.h"
#include "nameable.h"

struct sprite;

enum alignmentn : unsigned char;
enum gendern : unsigned char;
enum dietyn : unsigned char;

struct npci : nameable, classa, coloration, statable {
	gendern		gender;
	alignmentn	alignment;
	dietyn		diety;
	const char*	res[3]; // Resources
	sprite*		sprites[3]; // Loaded resources
	sprite*		getres(int n);
};
