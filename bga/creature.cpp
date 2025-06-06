#include "advance.h"
#include "area.h"
#include "creature.h"
#include "math.h"
#include "modifier.h"
#include "pushvalue.h"
#include "rand.h"
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

static void apply_advance(variant v) {
	if(v.iskind<abilityi>()) {
		if(v.value >= SimpleWeaponMace && v.value <= MartialWeaponPolearm) {
			auto level = v.counter;
			if(!level)
				level = 1;
			if(player->basic.abilitites[v.value] < level)
				player->basic.abilitites[v.value] = level;
		} else
			script::run(v);
	} else
		script::run(v);
}

static void apply_advance(const variants& source) {
	pushvalue push_modifier(modifier, Permanent);
	pushvalue push_modifiers(apply_modifiers);
	for(auto v : source)
		apply_advance(v);
}

static void apply_advance(variant v, int level) {
	for(auto& e : bsdata<advancei>()) {
		if(e.parent == v && e.level == level)
			apply_advance(e.elements);
	}
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
	area_index = 0xFFFF;
}

static short unsigned random_portrait_no_party(gendern gender) {
	portraiti* exist[sizeof(party) / sizeof(party[0]) + 1];
	auto ps = exist;
	for(auto p : party) {
		if(p)
			*ps++ = bsdata<portraiti>::elements + p->portrait;
	}
	return random_portrait(gender, exist);
}

static void raise_hit_points(classn v) {
	if(player->getlevel() == 1 && ischaracter(v))
		player->basic.abilitites[HitPoints] += bsdata<classi>::elements[v].hit_points;
	else
		player->basic.abilitites[HitPoints] += xrand(1, bsdata<classi>::elements[v].hit_points);
}

static void raise_class(classn classv) {
	variant v = bsdata<classi>::elements + classv;
	player->classes[classv] = player->classes[classv] + 1;
	apply_advance(v, player->classes[classv]);
}

static int get_maximum_rang() {
	return player->getlevel() + 3;
}

static bool is_class_skill(const void* object) {
	auto n = bsdata<skilli>::source.indexof(object);
	if(n == -1)
		return false;
	return player->isclass((skilln)n);
}

static void raise_random_skills(int points) {
	collection<skilli> source;
	source.select();
	source.random();
	auto maximum_rang = get_maximum_rang();
	for(auto p : source) {
		if(points <= 0)
			break;
		auto v = (skilln)bsdata<skilli>::source.indexof(p);
		if(player->isclass(v)) {
			auto n = maximum_rang - player->basic.skills[v];
			if(n >= 0) {
				if(n > points)
					n = points;
				player->basic.skills[v] += n;
				points -= n;
			}
		} else {
			auto n = maximum_rang / 2 - player->basic.skills[v];
			if(n >= 0) {
				//if(n * 2 > points)
				//	n = points / 2;
				//player->basic.skills[v] += n;
				//points -= n * 2;
			}
		}
	}
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

static int get_skill_points(classn v) {
	auto n = bsdata<classi>::elements[v].skill_points;
	n += player->basic.getbonus(Intelligence);
	if(n < 1)
		n = 1;
	if(player->getlevel() == 1)
		n *= 4;
	return n;
}

void creature::create(racen race, gendern gender, classn classv, unsigned short portrait) {
	player = bsdata<creature>::add();
	player->clear();
	player->area_index = current_area;
	player->gender = gender;
	player->portrait = portrait;
	player->race = race;
	random_ability();
	apply_portraits();
	raise_class(classv);
	raise_random_skills(get_skill_points(classv));
	raise_hit_points(classv);
	finish();
}

void creature::create(gendern gender) {
	auto pi = random_portrait_no_party(gender);
	auto p = bsdata<portraiti>::elements + pi;
	create(p->race, p->gender, p->classv, pi);
}

bool creature::isclass(skilln v) const {
	for(auto i = (classn)0; i <= Wizard; i = (classn)(i + 1)) {
		if(classes[i]) {
			if(FGT(bsdata<classi>::elements[i].skills, v))
				return true;
		}
	}
	return false;
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
	strenght += player->get(WeightAllowance);
	player->allowed_weight = maptbl(heavy_load, strenght);
}

static void update_skills() {
	for(auto i = (skilln)0; i <= WildernessLore; i = (skilln)(i + 1))
		player->skills[i] += player->getbonus(bsdata<skilli>::elements[i].ability);
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
	update_skills();
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

bool creature::isparty() const {
	for(auto pc : party) {
		if(pc == this)
			return true;
	}
	return false;
}

bool creature::isusable(const item& it) const {
	return isallow(*this, it.geti().required);
}

creature* get_creature(const void* object) {
	auto i = bsdata<creature>::source.indexof(object);
	if(i != -1)
		return bsdata<creature>::elements + i;
	return 0;
}

short unsigned creature::getindex() const {
	return this - bsdata<creature>::elements;
}

int	creature::getspellslots(classn type, int spell_level) const {
	auto& ei = bsdata<classi>::elements[type];
	if(!ei.cast)
		return 0; // Not caster class
	auto cast_ability = get(ei.cast);
	if((cast_ability - 10) < spell_level)
		return 0; // Can't cast, because ability is low.
	auto ability_bonus = getbonus(ei.cast);
	auto bonus_spells = 0;
	if(ability_bonus > 0)
		bonus_spells = (ability_bonus + 4 - spell_level) / 4;
	auto class_level = classes[type];
	return 1 + bonus_spells;
}