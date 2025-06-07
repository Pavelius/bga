#pragma once

#include "class.h"
#include "flagable.h"
#include "nameable.h"
#include "variant.h"

enum abilityn : unsigned char;
enum spelln : unsigned short;
enum schooln : unsigned char;
enum rangen : unsigned char;

struct spelli : nameable, classa {
	int			avatar;
	schooln		school;
	rangen		range;
	abilityn	save;
	unsigned char save_difficult; // 1 - for half effect if saved, 0 - for negates effect if save
	variants	wearing, instant;
	spelln		getindex() const;
};
extern spelli* last_spell;
struct spellv {
	char		powers[512];
};
typedef flagable<16, unsigned> spellf;
struct spellbook : spellv {
	spellf known;
	unsigned short owner; // Creature index
	unsigned short type; // 0 - 999 Class index, 1000+ Domain index
	const char* getid() const;
	void clear();
	bool is(spelln v) const { return known.is(v); }
};
extern spellbook* last_spellbook;
spellbook* find_spellbook(short unsigned owner, unsigned short type);

void add_player_spellbooks();
void add_known_spells(classn type, int level);