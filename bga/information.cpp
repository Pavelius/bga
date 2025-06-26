#include "ability.h"
#include "advance.h"
#include "alignment.h"
#include "answers.h"
#include "calendar.h"
#include "command.h"
#include "creature.h"
#include "item.h"
#include "io_stream.h"
#include "math.h"
#include "saveheader.h"
#include "school.h"
#include "script.h"
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

static void addh(stringbuilder& sb, const char* id) {
	if(sb)
		sb.add("\n\n");
	sb.addn("###%1", getnm(id));
}

static void addv(stringbuilder& sb, const char* id, const char* value) {
	sb.addn("%1: %2", getnm(id), value);
}

template<class T> static void addv(stringbuilder& sb, const char* id, unsigned short v) {
	sb.addn("%1: %2", getnm(id), bsdata<T>::elements[v].getname());
}

template<class T> static void addv(stringbuilder& sb, const char* group, short unsigned value, int bonus) {
	auto pn = getnm(group);
	sb.addn(pn, bsdata<T>::elements[value].getname(), bonus);
}

static unsigned set_flags(int f1, int f2) {
	unsigned result = 0;
	for(auto i = f1; i <= f2; i++)
		result |= FG(i);
	return result;
}

static void adds(stringbuilder& sb, const array& source, const char* id, unsigned flags, int fb, int fe) {
	auto flags_default = set_flags(fb, fe);
	flags = (flags & flags_default);
	if(!flags)
		return;
	addh(sb, id);
	if(flags == flags_default) {
		sb.addn(getnm(ids("Any", id)));
		return;
	}
	for(auto i = fb; i <= fe; i++) {
		if(!FGT(flags, i))
			continue;
		auto pn = (nameable*)source.ptr(i);
		sb.addn(pn->getname());
	}
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
}

static void add_special(stringbuilder& sb, const char* id, const char* id_special) {
	auto pn = getnme(ids(id, id_special));
	if(!pn)
		return;
	addh(sb, id_special);
	sb.addn(pn);
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

static void addv(stringbuilder& sb, const dice& value) {
	sb.adds("%1id%2i", value.c, value.d);
	if(value.b)
		sb.add("%+1i", value.b);
}

static void addd(stringbuilder& sb, const char* id, const dice& value) {
	if(!value)
		return;
	sb.addn("%1:", getnm(id));
	addv(sb, value);
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

static void addclasses(stringbuilder& sb, const classa& source) {
	auto level = source.getlevel();
	if(!level)
		return;
	sb.addn("###"); sb.add(getnm("CharacterLevel"), level);
	for(auto i = (classn)0; i <= Wizard; i = (classn)(i + 1))
		addb(sb, bsdata<classi>::elements[i].id, source.classes[i]);
}

static void player_information(stringbuilder& sb) {
	addclasses(sb, *player);
	addh(sb, "CharacterRace");
	sb.addn("%1 %2", bsdata<racei>::elements[player->race].getname(), getnm(bsdata<genderi>::elements[player->gender].id));
	addh(sb, "Alignment");
	sb.addn(bsdata<alignmenti>::elements[player->alignment].getname());
	addh(sb, "Experience");
	addb(sb, "Current", player->experience, 0, false);
	addb(sb, "NextLevel", player->getnextlevel(), 0, false);
	addh(sb, "SavingThrows");
	for(auto i = Fortitude; i <= Will; i = (abilityn)(i + 1))
		addb(sb, i, player->get(i), false);
	addh(sb, "AbilityStatistic");
	addv(sb, "WeightAllowance", getkg(player->allowed_weight));
}

static void add_attack_info(stringbuilder& sb, const item& weapon, int attack_bonus) {
	weaponi info; player->getattack(info, weapon);
	info.bonus += attack_bonus;
	sb.addn("- %AttackMelee %+1i", info.bonus);
	sb.adds("("); addv(sb, info.damage); sb.addv(")", 0);
}

static void player_combat_information(stringbuilder& sb) {
	addh(sb, "WeaponStatistic");
	for(auto i = 0; i < 4; i++) {
		auto weapon = player->wears[QuickWeapon + i * 2];
		auto offhand = player->wears[QuickOffhand + i * 2];
		if(i > 0 && !weapon)
			continue;
		if(weapon)
			sb.addn(weapon.getname());
		else
			sb.addn("%UnarmedStrike");
		add_attack_info(sb, weapon, 0);
	}
}

static void player_skill_information(stringbuilder& sb) {
	addh(sb, "Skills");
	for(auto i = (skilln)0; i <= WildernessLore; i = (skilln)(i + 1)) {
		auto raw_level = player->basic.get(i);
		auto level = player->get(i);
		if(raw_level > 0)
			sb.addn("%1%+2i", bsdata<skilli>::elements[i].getname(), level);
	}
	addh(sb, "Feats");
	for(auto& e : bsdata<feati>()) {
		if(!e.is(GeneralFeat) || e.is(UpgradeFeat))
			continue;
		auto n = e.getindex();
		if(player->is(n)) {
			auto level = 1;
			while(bsdata<feati>::elements[n].upgrade && player->is(bsdata<feati>::elements[n].upgrade)) {
				level++;
				n = bsdata<feati>::elements[n].upgrade;
			}
			sb.addn(e.getname());
			if(level > 1)
				sb.adds("x%1i", level);
		}
	}
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
		addv(sb, "Required", bsdata<feati>::elements[ei.required].getname());
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

static void character_abilities(stringbuilder& sb) {
	addh(sb, "Abilities");
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
		addb(sb, i, player->basic.abilities[i]);
}

static void character_skills(stringbuilder& sb) {
	addh(sb, "Skills");
	for(auto i = (skilln)0; i <= WildernessLore; i = (skilln)(i + 1)) {
		if(!player->basic.skills[i])
			continue;
		addb(sb, bsdata<skilli>::elements[i].id, player->skills[i], "%+1i");
	}
}

static void character_feats(stringbuilder& sb) {
	addh(sb, "Feats");
	for(auto& e : bsdata<feati>()) {
		if(!e.is(GeneralFeat))
			continue;
		auto n = e.getindex();
		if(!player->basic.is(n))
			continue;
		sb.addn(e.getname());
	}
}

static void character_brief_info(stringbuilder& sb) {
	if(current_step > ChooseName)
		sb.addn("###%1", player->getname());
	if(current_step > ChooseGender)
		addv<genderi>(sb, "Gender", player->gender);
	if(current_step > ChooseRace)
		addv<racei>(sb, "Race", player->race);
	if(current_step > ChooseClass)
		addv<classi>(sb, "Class", player->getmainclass());
	if(current_step > ChooseAlignment)
		sb.addn(bsdata<alignmenti>::elements[player->alignment].getname());
	if(current_step > ChooseAbilities)
		character_abilities(sb);
	if(current_step > ChooseSkills) {
		character_skills(sb);
		character_feats(sb);
	}
}

static void add_advantage(stringbuilder& sb, variant v) {
	if(v.iskind<abilityi>())
		addv<abilityi>(sb, "StartAbility", v.value, v.counter);
	else if(v.iskind<skilli>())
		addv<skilli>(sb, "StartSkill", v.value, v.counter);
	else if(v.iskind<feati>())
		sb.addn(bsdata<feati>::elements[v.value].getname());
	else if(v.iskind<script>())
		sb.addn(getnm(bsdata<script>::elements[v.value].id));
}

static void add_value(stringbuilder& sb, variant v) {
	if(v.iskind<abilityi>())
		sb.addn("%1 %2i", bsdata<abilityi>::elements[v.value].getname(), v.counter);
	else if(v.iskind<skilli>())
		sb.addn("%1 %+2i", bsdata<skilli>::elements[v.value].getname(), v.counter);
	else if(v.iskind<feati>())
		sb.addn(bsdata<feati>::elements[v.value].getname());
	else if(v.iskind<script>())
		sb.addn(getnm(bsdata<script>::elements[v.value].id));
}

static void add_value(stringbuilder& sb, const char* id, variants source) {
	if(!source)
		return;
	addh(sb, id);
	for(auto v : source)
		add_value(sb, v);
}

static void add_advantages(stringbuilder& sb, variant parent) {
	for(auto& e : bsdata<advancei>()) {
		if(e.parent != parent)
			continue;
		for(auto v : e.elements)
			add_advantage(sb, v);
	}
}

template<> void ftinfo<racei>(const void* object, stringbuilder& sb) {
	auto p = (racei*)object;
	add_description(sb, p->id);
	addh(sb, "Description");
	if(p->favor)
		addv<classi>(sb, "FavorClass", p->favor);
	else
		addv(sb, "FavorClass", getnm("AnyClass"));
	add_advantages(sb, p);
}

template<> void ftinfo<classi>(const void* object, stringbuilder& sb) {
	auto p = (classi*)object;
	add_description(sb, p->id);
	addh(sb, "HitPoints"); sb.addn("+1d%1i %-PerLevel", p->hit_points);
	addh(sb, "SkillPoints"); sb.addn("%1i+%-IntellegenceBonus %-PerLevel", p->skill_points);
	adds(sb, bsdata<alignmenti>::source, "Alignment", p->alignment, 0, 8);
	adds(sb, bsdata<feati>::source, "ProficientWeapon", p->proficient, SimpleWeaponMace, MartialWeaponPolearm);
	adds(sb, bsdata<feati>::source, "ProficientArmor", p->proficient, ArmorProficiencyLight, ShieldProficiency);
}

template<> void ftinfo<skilli>(const void* object, stringbuilder& sb) {
	auto p = (skilli*)object;
	add_description(sb, p->id);
	sb.add("\n\n");
	addv<abilityi>(sb, "BasicAbility", p->ability);
	// sb.add("%BasicAbility: %1", bsdata<abilityi>::elements[p->ability].getname());
}

template<> void ftinfo<feati>(const void* object, stringbuilder& sb) {
	auto p = (feati*)object;
	add_description(sb, p->id);
	add_special(sb, p->id, "Benefit");
	add_special(sb, p->id, "Normal");
	add_value(sb, "Required", p->require);
}

static void answer_info(stringbuilder& sb) {
	for(auto& e : bsdata<varianti>()) {
		if(e.pstatus && e.source->have(current_answer)) {
			e.pstatus(current_answer, sb);
			return;
		}
	}
	auto p = (nameable*)current_answer;
	auto pn = getnme(ids(p->id, "Info"));
	if(pn)
		sb.addn(pn);
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
	{"PlayerCombatInformation", player_combat_information},
	{"PlayerSkillInformation", player_skill_information},
	{"SpellInformation", spell_information},
};
BSDATAF(stringvari)