#pragma once

#include "class.h"
#include "flagable.h"
#include "nameable.h"
#include "variant.h"

enum spelln : unsigned short;
enum schooln : unsigned char;

struct spelli : nameable, classa {
	int			avatar;
	schooln		school;
	variants	wearing, instant;
};
struct spellv {
	char		powers[512];
};
typedef flagable<16, unsigned> spellf;
struct spellbook : spellv {
	spellf known;
	unsigned short owner; // Creature index
	unsigned short type; // 0 - 999 Class index, 1000+ Domain index
	bool is(spelln v) const { return known.is(v); }
};
spellbook* find_spellbook(short unsigned owner, unsigned short type);
