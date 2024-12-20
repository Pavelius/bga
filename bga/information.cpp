#include "ability.h"
#include "alignment.h"
#include "creature.h"
#include "item.h"
#include "math.h"
#include "stringbuilder.h"

static void addv(stringbuilder& sb, const char* id, const char* value) {
	sb.addn("%1: %2", getnm(id), value);
}

static void addb(stringbuilder& sb, const char* id, int value, const char* format = 0, bool skip_zero = true) {
	if(skip_zero && !value)
		return;
	sb.addn("%1:", getnm(id));
	if(!format)
		format = "%1i";
	sb.adds(format, value);
}

static void addb(stringbuilder& sb, ability_s i, int value, bool skip_zero = true) {
	addb(sb, bsdata<abilityi>::elements[i].id, value, bsdata<abilityi>::elements[i].format, skip_zero);
}

void status_info() {
}

static void add_description(stringbuilder& sb, const char* id, const char* id_basic = 0) {
	auto pn = getnme(ids(id, "Info"));
	if(!pn && id_basic)
		pn = getnme(ids(id_basic, "Info"));
	if(!pn)
		pn = getnme("NoItemInfo");
	if(!pn)
		return;
	sb.addn(pn);
	sb.add("\n\n");
}

const char* getkg(int weight) {
	return str("%1i.%2i %Kg", weight / 2, (weight * 10 / 2) % 10);
}

static void add_statistics(stringbuilder& sb, const variants& source) {
	for(auto v : source) {
		if(v.iskind<abilityi>())
			addb(sb, bsdata<abilityi>::elements[v.value].id, v.counter, bsdata<abilityi>::elements[v.value].format);
	}
}

static void add_db(stringbuilder& sb, const char* id, int value) {
	if(!value)
		return;
	if(value == -1)
		value = 0;
	addb(sb, id, value, "%+1i");
}

static void addd(stringbuilder& sb, const char* id, const dice& value) {
	if(!value)
		return;
	sb.addn("%1:", getnm(id));
	sb.adds("%1id%2i", value.c, value.d);
	if(value.b)
		sb.add("%+1i", value.b);
}

static void addv(stringbuilder& sb, const dice& value, damage_s type) {
	sb.adds("%1id%2i", value.c, value.d);
	if(value.b)
		sb.add("%+1i", value.b);
	sb.adds(getnm(str("%1Damage", bsdata<damagei>::elements[type].id)));
}

static void addd(stringbuilder& sb, const char* id, const dice& value, damage_s type) {
	if(!value)
		return;
	sb.addn("%1:", getnm(id));
	addv(sb, value, type);
}

static void add_critical(stringbuilder& sb, int critical, int multiplier, unsigned flags) {
	sb.addn(getnm(critical == 20 ? "CriticalHitLine20" : "CriticalHitLine"), critical, multiplier);
}

static const char* getfeatname(ability_s i, int level) {
	static const char* armor_proficiency[] = {0, "LightArmorProficiency", "MediumArmorProficiency", "HeavyArmorProficiency"};
	switch(i) {
	case ArmorProficiency:
		return getnm(maptbl(armor_proficiency, level));
	default:
		return getnm(bsdata<abilityi>::elements[i].id);
	}
}

static const char* getfeatname(variant v) {
	if(v.iskind<abilityi>())
		return getfeatname((ability_s)v.value, v.counter);
	else
		return v.getname();
}

void item::getinfo(stringbuilder& sb) const {
	auto& ei = geti();
	add_description(sb, ei.id, ei.basic ? ei.basic->id : 0);
	addb(sb, "MagicBonus", ei.magic, "%+1i");
	if(ei.wear == QuickWeapon) {
		addb(sb, "AttackBonus", ei.weapon.bonus, "%+1i");
		addd(sb, "Damage", ei.weapon.damage, ei.weapon.type);
		if(ei.is(Flaming)) {
			sb.adds("%-And ");
			addv(sb, {1, 6}, Fire);
		}
		if(ei.is(Frost)) {
			sb.adds("%-And ");
			addv(sb, {1, 6}, Cold);
		}
		add_critical(sb, ei.getcritical(), ei.getmultiplier(), ei.flags);
	}
	add_statistics(sb, ei.wearing);
	add_db(sb, "MaxDexterityBonus", ei.max_dex_bonus);
	if(ei.required)
		addv(sb, "Required", getfeatname(ei.required));
	addv(sb, "Weight", getkg(ei.weight));
}

static void addh(stringbuilder& sb, const char* format, ...) {
	if(!format)
		return;
	sb.addn("**[");
	sb.addv(format, xva_start(format));
	sb.add("]**");
}

static void addend(stringbuilder& sb) {
	sb.addn("\n");
}

static void addclasses(stringbuilder& sb, const classa& source) {
	auto level = source.getlevel();
	if(!level)
		return;
	addh(sb, getnm("CharacterLevel"), level);
	for(auto i = (classn)0; i <= Wizard; i = (classn)(i + 1))
		addb(sb, bsdata<classi>::elements[i].id, source.classes[i]);
	addend(sb);
}

void creature::getinfo(stringbuilder& sb) const {
	addclasses(sb, *this);
	addh(sb, getnm("CharacterRace"));
	sb.addn("%1 %2", bsdata<racei>::elements[race].getname(), getnm(bsdata<genderi>::elements[gender].id));
	addend(sb);
	addh(sb, getnm("Alignment"));
	sb.addn(bsdata<alignmenti>::elements[alignment].getname());
	addend(sb);
	addh(sb, getnm("Experience"));
	addb(sb, "Current", experience, 0, false);
	addb(sb, "NextLevel", getnextlevel(), 0, false);
	addend(sb);
	addh(sb, getnm("SavingThrows"));
	for(auto i = Fortitude; i <= Will; i = (ability_s)(i + 1))
		addb(sb, i, get(i), false);
	addend(sb);
	addh(sb, getnm("AbilityStatistic"));
	addv(sb, "WeightAllowance", getkg(allowed_weight));
}

void creature::getskillsinfo(stringbuilder& sb) const {
	addh(sb, getnm("Skills"));
	for(auto i = (skill_s)0; i <= WildernessLore; i = (skill_s)(i + 1)) {
		auto raw_level = basic.get(i);
		auto level = get(i);
		if(raw_level > 0)
			sb.addn("%1%+2i", bsdata<skilli>::elements[i].getname(), level);
	}
	addend(sb);
	addh(sb, getnm("Feats"));
	for(auto i = ArmorProficiency; i <= MartialWeaponPolearm; i = (ability_s)(i + 1)) {
		auto level = get(i);
		if(level)
			sb.addn(getfeatname(i, level));
	}
	for(auto i = Alertness; i <= WhirlwindAttack; i = (feat_s)(i + 1)) {
		if(is(i))
			sb.addn(bsdata<feati>::elements[i].getname());
	}
	addend(sb);
}

void skilli::getinfo(stringbuilder& sb) const {
	// addh(sb, "%1 (%2)", getname(), bsdata<abilityi>::elements[ability].getname());
	add_description(sb, id);
	//sb.add("\n\n");
	sb.add("%BasicAbility: %1", bsdata<abilityi>::elements[ability].getname());
}