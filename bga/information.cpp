#include "ability.h"
#include "item.h"
#include "stringbuilder.h"

static void addv(stringbuilder& sb, const char* id, const char* value) {
	sb.addn("%1: %2", getnm(id), value);
}

static void addb(stringbuilder& sb, const char* id, int value, const char* format = 0) {
	if(!value)
		return;
	sb.addn("%1:", getnm(id));
	if(!format)
		format = "%1i";
	sb.adds(format, value);
}

void status_info() {
}

static void add_description(stringbuilder& sb, const char* id, const itemi* basic = 0) {
	auto pn = getdescription(id);
	if(!pn && basic)
		pn = getdescription(basic->id);
	if(!pn)
		pn = getdescription("NoItem");
	if(!pn)
		return;
	sb.addn(pn);
	sb.add("\n\n");
}

static const char* getkg(int weight) {
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

static const char* getfeatname(variant v) {
	static const char* armor_proficiency[] = {0, "LightArmorProficiency", "MediumArmorProficiency", "HeavyArmorProficiency"};
	if(v.iskind<abilityi>()) {
		switch(v.value) {
		case ArmorProficiency:
			return getnm(maptbl(armor_proficiency, v.counter));
		default:
			return getnm(bsdata<abilityi>::elements[v.value].id);
		}
	} else
		return v.getname();
}

void item::getinfo(stringbuilder& sb) const {
	auto& ei = geti();
	add_description(sb, ei.id, ei.basic);
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