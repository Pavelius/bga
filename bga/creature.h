#include "actor.h"
#include "portrait.h"
#include "spell.h"

#pragma once

enum alignmentn : unsigned char;
enum dietyn : unsigned char;

struct creature : actor, avatarable, spellv {
	statable		basic;
	alignmentn		alignment;
	dietyn			diety;
	char			initiative;
	short			hp, hp_max;
	short unsigned	name;
	unsigned		experience;
	explicit operator bool() const { return hp > 0; }
	void			select();
	void			clear();
	short unsigned	getindex() const;
	const char*		getname() const { return "Pavelius"; }
	int				getspellslots(classn type, int spell_level) const;
	bool			isclass(skilln v) const;
	bool			isparty() const;
	bool			isselected() const;
	bool			isusable(const item& it) const;
	void			update();
	void			update_abilities();
};
extern creature* player;
extern creature* party[6];
extern creature* party_selected[16];

creature* get_creature(const void* object);
creature* get_selected();

void check_combat();
void clear_selection();
void create_character(racen race, gendern gender, classn classv, unsigned short portrait);
void create_character(gendern gender);
void create_npc(point position, const char* id);
bool have_selection();
void select_all_party();