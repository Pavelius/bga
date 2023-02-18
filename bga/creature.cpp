#include "creature.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"

creature* party[6];
creature* player;
collection<creature> selected_creatures;

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

static void apply(variant v) {
	if(v.iskind<abilityi>()) {
		switch(modifier) {
		case Permanent: player->basic.abilitites[v.value] += v.counter; break;
		default: player->abilitites[v.value] += v.counter; break;
		}
	} else
		script::run(v);
}

static void apply(const variants& source) {
	pushvalue push_modifier(modifier, NoModifier);
	pushvalue push_modifiers(apply_modifiers);
	for(auto v : source)
		apply(v);
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
	player->classes[classv]++;
}

void creature::create(race_s race, gender_s gender, class_s classv, unsigned short portrait) {
	pushvalue push_player(player, this);
	clear();
	this->gender = gender;
	this->portrait = portrait;
	this->race = race;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		basic.abilitites[i] = 10;
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