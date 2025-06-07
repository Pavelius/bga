#include "actor.h"
#include "actable.h"
#include "collection.h"
#include "portrait.h"
#include "spell.h"

#pragma once

enum alignmentn : unsigned char;
enum dietyn : unsigned char;

struct creature : actor, avatarable, spellv {
	statable		basic;
	alignmentn		alignment;
	dietyn			diety;
	short			hp, hp_max;
	unsigned		experience;
	void			clear();
	static void		create(gendern gender);
	static void		create(racen race, gendern gender, classn classv, unsigned short portrait);
	short unsigned	getindex() const;
	int				getspellslots(classn type, int spell_level) const;
	bool			isclass(skilln v) const;
	bool			isparty() const;
	bool			isusable(const item& it) const;
	void			paint() const;
	void			update();
	void			update_abilities();
};
extern creature* player;
extern creature* party[6];
extern collection<creature> selected_creatures;

creature* get_creature(const void* object);
