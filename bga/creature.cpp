#include "advance.h"
#include "creature.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"

creature* party[6];
creature* player;
collection<creature> selected_creatures;

static int heavy_load[] = {
	5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
	115, 130, 150, 175, 200, 230, 260, 300, 350,
	400, 460, 520, 600, 700, 800, 920, 1040, 1200, 1400
};

template<typename T>
static void copy(T& e1, const T& e2) {
	e1 = e2;
}

static void apply_portraits() {
	auto pi = bsdata<portraiti>::elements + player->portrait;
	player->setcolor(pi->colors);
}

static void finish() {
	player->basic.abilitites[HitPoints] += 10;
	player->update();
	player->hp = player->hp_max;
}

template<> void fnscript<abilityi>(int value, int counter) {
	switch(modifier) {
	case Permanent: player->basic.abilitites[value] += counter; break;
	default: player->abilitites[value] += counter; break;
	}
}

template<> void fnscript<feati>(int value, int counter) {
	if(counter >= 0) {
		switch(modifier) {
		case Permanent: player->basic.feats.set(value); break;
		default: player->feats.set(value); break;
		}
	}
}

static void apply(const variants& source) {
	pushvalue push_modifier(modifier, NoModifier);
	pushvalue push_modifiers(apply_modifiers);
	for(auto v : source)
		script::run(v);
}

static void apply_advance(const variants& source) {
	pushvalue push_modifier(modifier, Permanent);
	pushvalue push_modifiers(apply_modifiers);
	for(auto v : source)
		script::run(v);
}

static void apply_advance(variant v, int level) {
	for(auto& e : bsdata<advancei>()) {
		if(e.parent == v && e.level == level)
			apply_advance(e.elements);
	}
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

static void raise_class(class_s classv) {
	variant v = bsdata<classi>::elements + classv;
	auto new_level = player->classes[classv] + 1;
	apply_advance(v, new_level);
	player->classes[classv] = new_level;
}

static int compare_char(const void* v1, const void* v2) {
	return *((char*)v2) - *((char*)v1);
}

static int roll_4d6() {
	char temp[4];
	for(auto& v : temp)
		v = xrand(1, 6);
	qsort(temp, sizeof(temp) / sizeof(temp[0]), sizeof(temp[0]), compare_char);
	return temp[0] + temp[1] + temp[2];
}

static void random_ability() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilitites[i] = roll_4d6();
}

void creature::create(race_s race, gender_s gender, class_s classv, unsigned short portrait) {
	pushvalue push_player(player, this);
	clear();
	this->gender = gender;
	this->portrait = portrait;
	this->race = race;
	random_ability();
	apply_portraits();
	raise_class(classv);
	finish();
}

void creature::create(gender_s gender) {
	auto pi = random_portrait_no_party(gender);
	auto p = bsdata<portraiti>::elements + pi;
	create(p->race, p->gender, p->classv, pi);
}

static void update_wears() {
	for(auto& e : player->equipment()) {
		if(e)
			apply(e.geti().wearing);
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

static void update_weight() {
	player->weight = 0;
	for(auto& e : player->wears) {
		if(e)
			player->weight += e.getweight();
	}
	auto strenght = player->get(Strenght);
	player->allowed_weight = maptbl(heavy_load, strenght);
}

void creature::update_abilities() {
	auto level = getlevel();
	// Armor class
	abilitites[DodgeBonus] += get_dex_bonus(getbonus(Dexterity), wears[Body].geti().max_dex_bonus);
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
	pushvalue push_player(player, this);
	copy(*static_cast<statable*>(this), basic);
	update_wears();
	update_abilities();
	update_weight();
}

static bool isallow(const statable& source, variant v) {
	if(!v)
		return true;
	auto level = v.counter;
	if(!level)
		level = 1;
	if(v.iskind<abilityi>())
		return source.abilitites[v.value] >= level;
	else if(v.iskind<feati>())
		return source.feats.is((feat_s)v.value);
	return true;
}

bool creature::isusable(const item& it) const {
	return isallow(*this, it.geti().required);
}