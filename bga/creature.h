#pragma once

#include "actor.h"
#include "portrait.h"
#include "spell.h"

enum alignmentn : unsigned char;
enum dietyn : unsigned char;

struct creature : actor, statable, classa, avatarable, spellv {
	racen			race;
	gendern			gender;
	alignmentn		alignment;
	dietyn			diety;
	statable		basic;
	featf			feats;
	char			initiative;
	short			hp, hp_max;
	unsigned		experience;
	resname			name;
	short unsigned	speak;
	explicit operator bool() const { return hp > 0; }
	void			select();
	void			clear();
	void			fixattack(drawable* target);
	void			fixdamage();
	void			getattack(weaponi& result, const item& weapon) const;
	short unsigned	getindex() const;
	int				getmps() const { return 74; }
	const char*		getname() const { return name; }
	int				getspellslots(classn type, int spell_level) const;
	bool			is(featn v) const { return feats.is(v); }
	bool			isallow(featn v) const;
	bool			isclass(skilln v) const;
	bool			isparty() const;
	bool			isselected() const;
	bool			isusable(const item& it) const;
	void			nextaction();
	bool			roll(skilln value, int bonus = 0); // Default roll against difficult 10.
	void			set(featn v) { feats.set(v); }
	void			set(animaten v) { actor::set(v); }
	void			setreverse(animaten v);
	void			stop();
	void			update();
	void			updateanimate();
};
extern creature* player;
extern creature* party[6];
extern creature* party_selected[16];

creature* get_creature(const void* object);
creature* get_selected();

int skill_points_per_level(classn v);

void apply_portraits();
void check_combat();
void clear_selection();
void create_abilities(bool allow_random);
void create_character(racen race, gendern gender, classn classv, unsigned short portrait);
bool have_multiselect();
bool have_selection();
void party_add_item(item& v);
void player_finish();
void raise_class(classn classv);
void raise_race(racen race);
void select_all_party();
void update_player();