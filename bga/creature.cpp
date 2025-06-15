#include "advance.h"
#include "area.h"
#include "collection.h"
#include "creature.h"
#include "math.h"
#include "modifier.h"
#include "npc.h"
#include "pushvalue.h"
#include "rand.h"
#include "script.h"
#include "view.h"

creature* party[6];
creature* player;
creature* party_selected[16];
collection<creature> combatants;

static int heavy_load[] = {
	5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
	115, 130, 150, 175, 200, 230, 260, 300, 350,
	400, 460, 520, 600, 700, 800, 920, 1040, 1200, 1400
};

void clear_selection() {
	memset(party_selected, 0, sizeof(party_selected));
}

creature* get_selected() {
	for(auto p : party_selected) {
		if(p)
			return p;
	}
	return 0;
}

void select_all_party() {
	clear_selection();
	for(auto p : party)
		p->select();
}

bool have_multiselect() {
	return party_selected[0] && party_selected[1];
}

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
	player->stop();
}

template<> void fnscript<abilityi>(int value, int counter) {
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += counter; break;
	default: player->abilities[value] += counter; break;
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
			if(player->basic.abilities[v.value] < level)
				player->basic.abilities[v.value] = level;
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
	move_order = 0xFFFF;
	npc = 0xFFFF;
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
		player->basic.abilities[HitPoints] += bsdata<classi>::elements[v].hit_points;
	else
		player->basic.abilities[HitPoints] += xrand(1, bsdata<classi>::elements[v].hit_points);
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
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
		player->basic.abilities[i] = roll_4d6();
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

void create_npc(point position, const char* id) {
	player = 0;
	auto pn = bsdata<npci>::find(id);
	if(!pn)
		return;
	player = bsdata<creature>::add();
	player->clear();
	player->npc = getbsi(pn);
	player->area_index = current_area;
	player->gender = pn->gender;
	player->portrait = 0xFFFF;
	player->race = Human;
	copy(player->basic, *((statable*)pn));
	for(auto i = Commoner; i <= Wizard; i = (classn)(i + 1)) {
		if(!pn->classes[i])
			continue;
		for(auto n = 0; n < pn->classes[i]; n++) {
			raise_class(i);
			raise_hit_points(i);
		}
	}
	finish();
	player->setposition(position);
}

void create_character(racen race, gendern gender, classn classv, unsigned short portrait) {
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

void create_character(gendern gender) {
	auto pi = random_portrait_no_party(gender);
	auto p = bsdata<portraiti>::elements + pi;
	create_character(p->race, p->gender, p->classv, pi);
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
	abilities[DodgeBonus] += get_dex_bonus(getbonus(Dexterity), wears[Body].geti().max_dex_bonus);
	abilities[AC] += 10;
	abilities[AC] += abilities[DodgeBonus];
	abilities[AC] += abilities[ArmorBonus];
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
		return source.abilities[v.value] >= level;
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

bool creature::isselected() const {
	for(auto ps : party_selected) {
		if(ps == this)
			return true;
	}
	return false;
}

bool creature::isusable(const item& it) const {
	return isallow(*this, it.geti().required);
}

void creature::select() {
	if(isselected())
		return;
	for(auto& e : party_selected) {
		if(!e) {
			e = this;
			break;
		}
	}
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

static bool local_creature(const void* object) {
	auto p = (creature*)object;
	if(p->hp <= 0)
		return false;
	return p->ispresent();
}

static bool enemy_combatants_present() {
	for(auto p : combatants) {
		if(p->is(Enemy))
			return true;
	}
	return false;
}

static void check_initiative() {
	for(auto p : combatants) {
		if(p->initiative == -100)
			p->initiative = d20() + p->abilities[Initiative];
	}
}

static int compare_initiative(const void* v1, const void* v2) {
	auto p1 = *((creature**)v1);
	auto p2 = *((creature**)v2);
	return p1->initiative - p2->initiative;
}

static void play_human_action() {
	auto object = choose_combat_action();
	if(object >= area_cost && object < area_cost + lenghtof(area_cost)) {
		auto move_index = (short unsigned*)object - area_cost;
		auto move_position = s2a(i2s(move_index), player->getsize());
		player->lookat(move_position);
		player->setposition(move_position);
	} else if(bsdata<creature>::have(object)) {

	}
}

static void play_combat_round() {
	auto push_player = player;
	for(auto p : combatants) {
		player = p;
		if(player->isparty()) {
			play_human_action();
		} else if(player->is(Enemy)) {

		} else {
			// Neutral do nothing
		}
	}
}

void check_combat() {
	combatants.select(local_creature);
	while(enemy_combatants_present()) {
		check_initiative();
		combatants.sort(compare_initiative);
		play_combat_round();
	}
}