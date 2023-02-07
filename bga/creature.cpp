#include "creature.h"

creature* last_creature;

template<typename T>
static void copy(T& e1, const T& e2) {
	e1 = e2;
}

static void apply_portraits(creature* p) {
	auto pi = bsdata<portraiti>::elements + p->portrait;
	memcpy(p->colors, pi->colors, sizeof(pi->colors));
}

static void finish(creature* p) {
	p->update();
	p->hp = p->hp_max;
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::create(gender_s gender) {
	this->gender = gender;
	this->portrait = random_portrait(gender);
	apply_portraits(this);
	finish(this);
}

void creature::update_abilities() {
	auto level = 1;
	abilitites[AC] += getbonus(Dexterity);
	abilitites[AC] += 10;
	hp_max = get(HitPoints) + level * getbonus(Constitution);
	if(hp_max < level)
		hp_max = level;
	if(hp > hp_max)
		hp = hp_max;
}

void creature::update() {
	copy(*static_cast<statable*>(this), basic);
	update_abilities();
}