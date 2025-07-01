#include "advance.h"
#include "area.h"
#include "collection.h"
#include "condition.h"
#include "creature.h"
#include "math.h"
#include "modifier.h"
#include "npc.h"
#include "order.h"
#include "party.h"
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

static int get_base_save(int level, abilityn save, unsigned good_saves) {
	if(!level)
		return 0;
	else if(FGT(good_saves, save))
		return (level + 4) / 2;
	else if(level > 2)
		return (level - 1) / 2;
	else
		return 0;
}

static int get_base_attack(int level, int attack_type) {
	switch(attack_type) {
	case 1: return level;
	case 2: return level / 2;
	default: return level / 3;
	}
}

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

void apply_portraits() {
	auto pi = bsdata<portraiti>::elements + player->portrait;
	player->setcolor(pi->colors);
	if(pi->sound)
		player->speak.set(pi->sound);
}

void player_finish() {
	player->update();
	player->hp = player->hp_max;
	player->stop();
}

static void apply(const variants& source) {
	pushvalue push_modifier(modifier, NoModifier);
	for(auto v : source)
		script_run(v);
}

static void apply_advance(const variants& source) {
	pushvalue push_modifier(modifier, Permanent);
	for(auto v : source)
		script_run(v);
}

static void apply_advance(variant v, int level) {
	v.counter = level;
	for(auto& e : bsdata<advancei>()) {
		if(e.parent == v)
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

void raise_class(classn classv) {
	auto& ei = bsdata<classi>::elements[classv];
	variant v = bsdata<classi>::elements + classv;
	auto prev_level = player->classes[classv];
	player->classes[classv] += 1;
	auto level = player->classes[classv];
	if(level == 1)
		player->basic.feats.add(0, ei.proficient);
	apply_advance(v, player->classes[classv]);
	if(player->getlevel() == 1 && ischaracter(classv))
		player->basic.abilities[HitPoints] += ei.hit_points;
	else
		player->basic.abilities[HitPoints] += xrand(1, ei.hit_points);
	player->basic.abilities[Attack] += get_base_attack(level, ei.attack) - get_base_attack(prev_level, ei.attack);
	player->basic.abilities[Fortitude] += get_base_save(level, Fortitude, ei.saves) - get_base_save(prev_level, Fortitude, ei.saves);
	player->basic.abilities[Reflexes] += get_base_save(level, Reflexes, ei.saves) - get_base_save(prev_level, Reflexes, ei.saves);
	player->basic.abilities[Will] += get_base_save(level, Will, ei.saves) - get_base_save(prev_level, Will, ei.saves);
}

void raise_race(racen race) {
	player->race = race;
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
		player->basic.abilities[i] = 8;
	apply_advance(bsdata<racei>::elements + player->race, 0);
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
		for(auto n = 0; n < pn->classes[i]; n++)
			raise_class(i);
	}
	player_finish();
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
	player_finish();
}

void create_party_character(int index) {
	auto& e = last_party->characters[index];
	player = bsdata<creature>::add();
	player->clear();
	player->area_index = current_area;
	player->gender = e.gender;
	player->portrait = e.portrait;
	player->alignment = e.alignment;
	player->race = e.race;
	player->name = e.name;
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
		player->basic.abilities[i] = e.abilities[i - Strenght];
	apply_portraits();
	raise_class(e.type);
	for(auto i = (skilln)0; i <= WildernessLore; i = (skilln)(i + 1))
		player->basic.skills[i] = e.skills[i];
	player->basic.feats.add(e.feats);
	player_finish();
}

void create_character(gendern gender) {
	auto pi = random_portrait_no_party(gender);
	auto p = bsdata<portraiti>::elements + pi;
	create_character(p->race, p->gender, p->classv, pi);
}

bool creature::isclass(skilln v) const {
	for(auto i = (classn)0; i <= Wizard; i = (classn)(i + 1)) {
		if(classes[i]) {
			if(bsdata<classi>::elements[i].is(v))
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
	// Attacks
	abilities[AttackMelee] += getbonus(Strenght);
	abilities[DamageMelee] += getbonus(Strenght);
	abilities[AttackRanged] += getbonus(Dexterity);
	// Saves
	abilities[Fortitude] += getbonus(Constitution);
	abilities[Reflexes] += getbonus(Dexterity);
	abilities[Will] += getbonus(Wisdow);
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
	auto n = it.geti().required;
	if(!n)
		return true;
	return basic.is(n);
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
			p->initiative = d20() + p->getbonus(Dexterity);
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

int skill_points_per_level(classn v) {
	auto n = player->basic.getbonus(Intelligence);
	n += bsdata<classi>::elements[v].skill_points;
	return n;
}

static bool is_allow(creature* p, variants source) {
	pushvalue push_player(player, p);
	for(auto v : source) {
		if(v.iskind<abilityi>()) {
			if(p->basic.abilities[v.value] < v.counter)
				return false;
		} else if(v.iskind<feati>()) {
			if(!p->basic.feats.is(v.value))
				return false;
		} else if(v.iskind<conditioni>()) {
			if(!bsdata<conditioni>::elements[v.value].proc())
				return false;
		}
	}
	return true;
}

bool creature::isallow(featn v) const {
	if(basic.is(v))
		return true;
	return is_allow(const_cast<creature*>(this), bsdata<feati>::elements[v].require);
}

void creature::getattack(weaponi& result, const item& weapon) const {
	if(weapon)
		result = weapon.geti().weapon;
	else {
		result.clear();
		result.damage.c = 1;
		result.damage.d = 2;
	}
	if(true) { // Melee attack
		result.damage.b += get(DamageMelee);
		result.bonus += get(Attack);
		result.bonus += get(AttackMelee);
	}
}