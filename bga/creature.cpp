#include "modifier.h"
#include "creature.h"

creature* party[6];
creature* player;

template<typename T>
static void copy(T& e1, const T& e2) {
	e1 = e2;
}

static void apply_portraits(creature* p) {
	auto pi = bsdata<portraiti>::elements + p->portrait;
	p->setcolor(pi->colors);
}

static void finish(creature* p) {
	p->basic.abilitites[HitPoints] += 10;
	p->update();
	p->hp = p->hp_max;
}

static void apply_element(variant v) {
	if(v.iskind<abilityi>()) {
		switch(modifier) {
		case Permanent: player->basic.abilitites[v.value] += v.counter; break;
		default: player->abilitites[v.value] += v.counter; break;
		}
	} else if(v.iskind<modifieri>()) {
		modifier = (modifier_s)v.value;
	}
}

void creature::apply(const variants& source) {
	auto push_modifier = modifier; modifier = NoModifier;
	auto push_modifiers = apply_modifiers; apply_modifiers = 0;
	auto push_player = player; player = this;
	for(auto v : source)
		apply_element(v);
	player = push_player;
	modifier = push_modifier;
	apply_modifiers = push_modifiers;
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

static short unsigned random_portrait_no_party(gender_s gender) {
	portraiti* exist[sizeof(party) / sizeof(party[0]) + 1];
	auto ps = exist;
	for(auto p : party) {
		if(p)
			*ps++ = bsdata<portraiti>::elements + p->portrait;
	}
	return random_portrait(gender, exist);
}

static void raise_class(creature* player, class_s classv) {
	player->classes[classv]++;
}

void creature::create(race_s race, gender_s gender, class_s classv, unsigned short portrait) {
	clear();
	this->gender = gender;
	this->portrait = portrait; // random_portrait_no_party(gender);
	this->race = race;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		basic.abilitites[i] = 10;
	apply_portraits(this);
	raise_class(this, classv);
	finish(this);
}

void creature::create(gender_s gender) {
	auto pi = random_portrait_no_party(gender);
	auto p = bsdata<portraiti>::elements + pi;
	create(p->race, p->gender, p->classv, pi);
}

static void update_wears(creature* p) {
	for(auto& e : p->equipment()) {
		if(e)
			p->apply(e.geti().wearing);
	}
}

static int get_dex_bonus(int dex_bonus, int max_dex_bonus) {
	if(max_dex_bonus) {
		if(max_dex_bonus == -1)
			max_dex_bonus = 0;
		if(dex_bonus > max_dex_bonus)
			dex_bonus = max_dex_bonus;
	}
	return dex_bonus;
}

void creature::update_abilities() {
	auto level = 1;
	// Armor class
	abilitites[DodgeBonus] += get_dex_bonus(getbonus(Dexterity), wears[Body].geti().max_dex_bonus),
		abilitites[AC] += 10;
	abilitites[AC] += abilitites[DodgeBonus];
	abilitites[AC] += abilitites[ArmorBonus];
	// Hit points
	hp_max = get(HitPoints) + level * getbonus(Constitution);
	if(hp_max < level)
		hp_max = level;
	if(hp > hp_max)
		hp = hp_max;
}

void creature::update() {
	copy(*static_cast<statable*>(this), basic);
	update_wears(this);
	update_abilities();
}