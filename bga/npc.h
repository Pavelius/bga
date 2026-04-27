#pragma once

#include "ability.h"
#include "coloration.h"
#include "class.h"
#include "nameable.h"
#include "variant.h"

struct sprite;
struct rfpma;

enum alignmentn : unsigned char;
enum gendern : unsigned char;
enum dietyn : unsigned char;
enum racen : unsigned char;

struct npci : nameable, classa, coloration, statable {
	gendern		gender;
	alignmentn	alignment;
	dietyn		diety;
	racen		race;
	char		cr;
	rfpma*		resid[2]; // Resources
	variants	elements;
	sprite* getres(int n);
};
void clear_npc_sprites();