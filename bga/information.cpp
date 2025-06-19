#include "ability.h"
#include "alignment.h"
#include "answers.h"
#include "calendar.h"
#include "creature.h"
#include "item.h"
#include "io_stream.h"
#include "math.h"
#include "saveheader.h"
#include "school.h"
#include "stringbuilder.h"
#include "stringvar.h"

namespace {
struct gender_change_string {
	const char*	female;
	const char*	male;
	const char*	multiply;
};
static gender_change_string player_gender[] = {
	{"госпожа", "господин", "господа"},
	{"женщина", "мужчина", "господа"},
	{"стерва", "ублюдок", "ублюдки"},
	{"миледи", "милорд", "милорды"},
	{"леди", "лорд", "лорды"},
	{"такая", "такой", "такие"},
	{"она", "он", "они"},
	{"шла", "шел", "шли"},
	{"нее", "него", "них"},
	{"ась", "ся", "ись"},
	{"ая", "ый", "ые"},
	{"ей", "ему", "им"},
	{"ла", "", "ли"},
	{"ее", "его", "их"},
	{"а", "", "и"},
};
}

static void addv(stringbuilder& sb, const char* id, const char* value) {
	sb.addn("%1: %2", getnm(id), value);
}

template<class T> void addv(stringbuilder& sb, const char* id, unsigned short v) {
	sb.addn("%1: %2", getnm(id), bsdata<T>::elements[v].getname());
}

static void addb(stringbuilder& sb, const char* id, int value, const char* format = 0, bool skip_zero = true) {
	if(skip_zero && !value)
		return;
	sb.addn("%1:", getnm(id));
	if(!format)
		format = "%1i";
	sb.adds(format, value);
}

static void addb(stringbuilder& sb, abilityn i, int value, bool skip_zero = true) {
	addb(sb, bsdata<abilityi>::elements[i].id, value, bsdata<abilityi>::elements[i].format, skip_zero);
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

static void addv(stringbuilder& sb, const dice& value, damagen type) {
	sb.adds("%1id%2i", value.c, value.d);
	if(value.b)
		sb.add("%+1i", value.b);
	sb.adds(getnm(str("%1Damage", bsdata<damagei>::elements[type].id)));
}

static void addd(stringbuilder& sb, const char* id, const dice& value, damagen type) {
	if(!value)
		return;
	sb.addn("%1:", getnm(id));
	addv(sb, value, type);
}

static void add_critical(stringbuilder& sb, int critical, int multiplier, unsigned flags) {
	sb.addn(getnm(critical == 20 ? "CriticalHitLine20" : "CriticalHitLine"), critical, multiplier);
}

static const char* getfeatname(abilityn i, int level) {
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
		return getfeatname((abilityn)v.value, v.counter);
	else
		return v.getname();
}

static void addh(stringbuilder& sb, const char* format, ...) {
	if(!format)
		return;
	XVA_FORMAT(format);
	sb.addn("**[");
	sb.addv(format, format_param);
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

static void player_information(stringbuilder& sb) {
	addclasses(sb, *player);
	addh(sb, getnm("CharacterRace"));
	sb.addn("%1 %2", bsdata<racei>::elements[player->race].getname(), getnm(bsdata<genderi>::elements[player->gender].id));
	addend(sb);
	addh(sb, getnm("Alignment"));
	sb.addn(bsdata<alignmenti>::elements[player->alignment].getname());
	addend(sb);
	addh(sb, getnm("Experience"));
	addb(sb, "Current", player->experience, 0, false);
	addb(sb, "NextLevel", player->getnextlevel(), 0, false);
	addend(sb);
	addh(sb, getnm("SavingThrows"));
	for(auto i = Fortitude; i <= Will; i = (abilityn)(i + 1))
		addb(sb, i, player->get(i), false);
	addend(sb);
	addh(sb, getnm("AbilityStatistic"));
	addv(sb, "WeightAllowance", getkg(player->allowed_weight));
}

static void player_skill_information(stringbuilder& sb) {
	addh(sb, getnm("Skills"));
	for(auto i = (skilln)0; i <= WildernessLore; i = (skilln)(i + 1)) {
		auto raw_level = player->basic.get(i);
		auto level = player->get(i);
		if(raw_level > 0)
			sb.addn("%1%+2i", bsdata<skilli>::elements[i].getname(), level);
	}
	addend(sb);
	addh(sb, getnm("Feats"));
	for(auto i = ArmorProficiency; i <= MartialWeaponPolearm; i = (abilityn)(i + 1)) {
		auto level = player->get(i);
		if(level)
			sb.addn(getfeatname(i, level));
	}
	for(auto i = Alertness; i <= WhirlwindAttack; i = (feat_s)(i + 1)) {
		if(player->is(i))
			sb.addn(bsdata<feati>::elements[i].getname());
	}
	addend(sb);
}

static void item_name(stringbuilder& sb) {
	sb.add(last_item->getname());
}

static void addv(stringbuilder& sb, const classa& v) {
	auto record_count = 0;
	for(auto& e : bsdata<classi>()) {
		auto i = e.getindex();
		if(i == Sorcerer)
			continue;
		if(!v.classes[i])
			continue;
		if(!record_count) {
			sb.addn(getnm("Levels"));
			sb.add(":");
		} else
			sb.add(",");
		sb.adds(e.getshortname());
		sb.adds("%1i", v.classes[i]);
		record_count++;
	}
}

static void add_saving_throws(stringbuilder& sb) {
	if(last_spell->save) {
		addv(sb, "SavingThrows", bsdata<abilityi>::elements[last_spell->save].getname());
		switch(last_spell->save_difficult) {
		case 1: sb.adds(getnm("SaveHalf")); break;
		default: sb.adds(getnm("SaveCancel")); break;
		}
	} else
		addv(sb, "SavingThrows", getnm("No"));
}

static void spell_information(stringbuilder& sb) {
	addv<schooli>(sb, "School", last_spell->school);
	addv(sb, *last_spell);
	addv<rangei>(sb, "Range", last_spell->range);
	add_saving_throws(sb);
}

static void item_information(stringbuilder& sb) {
	auto& ei = last_item->geti();
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

static void passed_time(stringbuilder& sb) {
	auto h = gethour();
	auto d = getday();
	auto m = getminute();
	if(d)
		sb.adds("%1i %2", d, getnm(str_count("Day", d)));
	if(h)
		sb.adds("%1i %2", h, getnm(str_count("Hour", h)));
	if(!d && m)
		sb.adds("%1i %2", h, getnm(str_count("Minute", m)));
}

static void real_time(stringbuilder& sb, const io::file::datei& v) {
	sb.adds("%1i", v.day);
	sb.adds("%-1", getnm(get_real_month_of(v.month)));
	sb.adds("%1i", v.year);
	sb.add(", %1.2i:%2.2i", v.hour, v.minute);
}

static void real_time(stringbuilder& sb) {
	real_time(sb, last_save_header->change);
}

static void game_version(stringbuilder& sb) {
	sb.add(getnm("Version"));
	sb.adds("%1i.%2i", 0, 1);
}

static void character_brief_info(stringbuilder& sb) {
	addv<genderi>(sb, "Gender", player->gender);
}

static void add_race_info(stringbuilder& sb, racei* p) {
}

static void answer_info(stringbuilder& sb) {
	auto p = (nameable*)current_answer;
	auto pn = getnme(ids(p->id, "Info"));
	if(pn)
		sb.addn(pn);
	if(bsdata<racei>::have(pn))
		add_race_info(sb, (racei*)p);
}

template<> void ftinfo<skilli>(const void* object, stringbuilder& sb) {
	auto p = (skilli*)object;
	add_description(sb, p->id);
	sb.add("%BasicAbility: %1", bsdata<abilityi>::elements[p->ability].getname());
}

static bool creature_identifier(stringbuilder& sb, const char* identifier) {
	if(!player)
		return false;
	if((equal(identifier, "герой") || equal(identifier, "name"))) {
		sb.add(player->getname());
		return true;
	} else if(equal(identifier, "героя")) {
		sb.addof(player->getname());
		return true;
	} else if(equal(identifier, "герою")) {
		sb.addto(player->getname());
		return true;
	} else {
		for(auto& e : player_gender) {
			if(!equal(e.female, identifier))
				continue;
			if(player->gender == NoGender)
				sb.add(e.multiply);
			else if(player->gender == Female)
				sb.add(e.female);
			else
				sb.add(e.male);
			return true;
		}
	}
	return false;
}

void main_identifier(stringbuilder& sb, const char* identifier) {
	if(creature_identifier(sb, identifier))
		return;
	if(stringvar_identifier(sb, identifier))
		return;
	default_string(sb, identifier);
}

BSDATA(stringvari) = {
	{"AnswerInfo", answer_info},
	{"CharacterBriefInfo", character_brief_info},
	{"ItemInformation", item_information},
	{"ItemName", item_name},
	{"GameVersion", game_version},
	{"PassedTime", passed_time},
	{"RealTime", real_time},
	{"PlayerInformation", player_information},
	{"PlayerSkillInformation", player_skill_information},
	{"SpellInformation", spell_information},
};
BSDATAF(stringvari)