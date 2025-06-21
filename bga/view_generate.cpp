#include "alignment.h"
#include "answers.h"
#include "audio.h"
#include "command.h"
#include "creature.h"
#include "draw.h"
#include "math.h"
#include "portrait.h"
#include "rand.h"
#include "skill.h"
#include "vector.h"
#include "view.h"

using namespace draw;

static vector<portraiti*> portraits;
static vector<nameable*> records;

static unsigned current_value;
static char ability_normal[Charisma + 1];
static char ability_cost[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 13, 16, 20, 25};
static char current_skills[WildernessLore + 1];
static char feat_points;
static featf current_feats;

const int ability_points_maximum = 22;

static int get_ability_spend() {
	auto r = 0;
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1)) {
		auto d = player->basic.abilities[i] - ability_normal[i];
		if(d < 0)
			r -= maptbl(ability_cost, -d);
		else
			r += maptbl(ability_cost, d);
	}
	return r;
}

static bool have_party(int index) {
	for(auto p : party) {
		if(p && p->portrait == index)
			return true;
	}
	return false;
}

static void select_portrait(gendern gender, int current) {
	portraits.clear();
	for(auto& e : bsdata<portraiti>()) {
		if(e.gender != gender)
			continue;
		auto index = e.getindex();
		if(index == current)
			current_value = portraits.getcount();
		else if(have_party(index))
			continue;
		portraits.add(&e);
	}
}

static void paint_portrait() {
	if(player->portrait)
		image(23, 151, gres(PORTL), player->portrait, 0);
}

static void paint_character_generation(const char* id) {
	setdialog(174, 22, 452, 29); texta(STONEBIG, getnm("CharacterGeneration"), AlignCenterCenter);
	setdialog(173, 65, 453, 20); texta(getnm(id), AlignCenterCenter);
}

static void paint_steps() {
	setdialog(274, 113);
	for(auto i = ChooseGender; i <= ChooseName; i = (commandn)(i + 1)) {
		auto b = (i % 4) * 4;
		button(GBTNLRG, b + 1, b + 2, '1' + i, bsdata<commandi>::elements[i].id, b + 3, current_step == i);
		fire(buttonparam, (long)(bsdata<commandi>::elements + i));
		caret.y += 35;
	}
}

static void pick_answer() {
	auto p = (nameable*)hot.object;
	current_answer = p;
	set_description("%AnswerInfo");
}

static void paint_answers(int bh) {
	for(auto& e : an) {
		auto i = an.indexof(&e);
		auto b = (i % 4) * 4;
		auto c = b + 1;
		if(current_answer == e.value)
			c = b + 0;
		button(GBTNLRG, c, b + 2, '1' + i, e.text, b + 3, true, false);
		fire(pick_answer, 0, 0, e.value);
		caret.y += bh;
	}
}

static void paint_answers() {
	setdialog(274, 113);
	paint_answers(35);
}

static void set_step_description() {
	if(current_step == ChooseGender)
		set_description(getnm("GenerationMainInfo"));
	else
		set_description("%CharacterBriefInfo");
}

static void start_over() {
	player->clear();
	current_step = ChooseGender;
	set_step_description();
}

static void back_one_step() {
	if(current_step == ChooseGender)
		return;
	current_step = (commandn)(current_step - 1);
	set_step_description();
}

static void paint_footer() {
	setdialog(35, 550); button(GBTNSTD, 1, 2, 0, "Biography", 3, false);
	setdialog(204, 550); button(GBTNSTD, 5, 6, 0, "Import");
	setdialog(342, 550); button(GBTNSTD, 9, 10, KeyEscape, "Back", 11, current_step != ChooseGender); fire(back_one_step);
	setdialog(478, 550); button(GBTNSTD, 1, 2, KeyEnter, "Finish", 3, false);
	setdialog(647, 550);
	if(current_step == ChooseGender) {
		button(GBTNSTD, 5, 6, 0, "Cancel");
		fire(buttoncancel);
	} else {
		button(GBTNSTD, 5, 6, 0, "StartOver");
		fire(start_over);
	}
}

static bool allow_next_button() {
	switch(current_step) {
	case ChooseAbilities: return get_ability_spend() == ability_points_maximum;
	default: return current_answer != 0;
	}
}

static void paint_footer_answer(bool enable_next) {
	setdialog(35, 550); button(GBTNSTD, 1, 2, 0, "Biography", 3, false);
	setdialog(204, 550); button(GBTNSTD, 5, 6, 0, "Import", 7, false);
	setdialog(342, 550); button(GBTNSTD, 9, 10, KeyEscape, "Back"); fire(buttoncancel);
	setdialog(478, 550); button(GBTNSTD, 1, 2, KeyEnter, "Next", 3, enable_next); fire(buttonok);
	setdialog(647, 550); button(GBTNSTD, 5, 6, 0, "Cancel", 7, false);
}

static void paint_description() {
	setdialog(570, 153, 184, 325);
	paint_description(11, -2, 5);
}

static void random_avatar() {
	current_value = xrand(0, portraits.count - 1);
}

static void paint_choose_avatar() {
	if(current_value >= portraits.count)
		current_value = portraits.count - 1;
	if(current_value < 0)
		current_value = 0;
	paint_game_dialog(GUICGB);
	paint_character_generation("ChooseAvatar");
	image(254, 87, gres(GUIACG), 1, 0);
	image(295, 116, gres(PORTL), portraits[current_value]->getindex(), 0);
	setdialog(262, 284); button(GBTNPOR, 0, 1, KeyLeft); fire(cbsetint, current_value - 1, 0, &current_value);
	setdialog(512, 284); button(GBTNPOR, 2, 3, KeyRight); fire(cbsetint, current_value + 1, 0, &current_value);
	setdialog(276, 394 + 87); button(GBTNLRG, 1, 2, 'R', "Random"); fire(random_avatar);
	paint_footer_answer(allow_next_button());
	paint_description();
	audio_update_channels();
}

static int cost_pay(int delta) {
	if(delta >= 10)
		return 100;
	auto n1 = ability_cost[delta];
	auto n2 = ability_cost[delta + 1];
	return n2 - n1;
}

static void tips_help(const char* id) {
	button_check(0);
	if(button_executed)
		set_description_id(id);
}

static void tips_help_button(const char* id) {
	if(button_executed)
		set_description_id(id);
}

static void paint_choose_ability() {
	auto ability_spent = get_ability_spend();
	auto ability_left = ability_points_maximum - ability_spent;
	image(254, 87, gres(GUIACG), 2, 0);
	setdialog(27, 57, 205, 28); tips_help("AbilityPointsLeft"); texta(getnm("AbilityPointsLeft"), AlignCenterCenter);
	setdialog(240, 57, 33, 28); tips_help("AbilityPointsLeft"); texta(str("%1i", ability_left), AlignCenterCenter);
	setdialog(24, 94, 120, 28);
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1)) {
		auto push_caret = caret; width = 120; height = 28;
		auto help_id = bsdata<abilityi>::elements[i].id;
		texta(bsdata<abilityi>::elements[i].getname(), AlignRightCenter);
		tips_help(help_id);
		caret.x = push_caret.x + 136; width = 33;
		auto value = player->basic.abilities[i];
		auto delta = value - ability_normal[i];
		texta(str("%1i", value), AlignCenterCenter);
		caret.x = push_caret.x + 176;
		texta(str("%1i", value / 2 - 5), AlignCenterCenter);
		auto b = 3 * ((i - Strenght) % 4);
		caret.y -= 3;
		caret.x = push_caret.x + 215;
		button(GBTNPLUS, b, b + 1, 0, 0, b + 2, cost_pay(delta) <= ability_left);
		tips_help_button(help_id);
		fire(cbsetchr, value + 1, 0, &player->basic.abilities[i]);
		caret.x = push_caret.x + 233;
		button(GBTNMINS, b, b + 1, 0, 0, b + 2, delta > 0);
		tips_help_button(help_id);
		fire(cbsetchr, value - 1, 0, &player->basic.abilities[i]);
		caret = push_caret;
		caret.y += 36;
	}
}

static void change_skill() {
	auto v = hot.param;
	auto n = (skilln)hot.param2;
	player->basic.skills[n] += (int)v;
	player->basic.abilities[SkillPoints] -= (int)((player->isclass(n) ? 1 : 2) * v);
}

static void change_feat() {
	auto v = hot.param;
	auto n = (featn)hot.param2;
	if(v >= 0)
		player->basic.feats.set(n);
	else
		player->basic.feats.remove(n);
	feat_points -= (char)v;
}

static void paint_skill_row(void* object) {
	auto p = (skilli*)object;
	auto n = p->getindex();
	auto b = 3 * (n % 3);
	auto v = player->basic.skills[n];
	auto c = player->isclass(n) ? 1 : 2;
	auto m = player->getmaxskill();
	auto skill_points = player->basic.abilities[SkillPoints];
	pushfore push_fore;
	pushrect push; height = 28;
	caret.x = push.caret.x + 206;
	button(GBTNPLUS, b + 0, b + 1, 0, 0, b + 2, (v < current_skills[n] + m) && skill_points >= c);
	fire(change_skill, 1, n);
	caret.x = push.caret.x + 224;
	button(GBTNMINS, b + 0, b + 1, 0, 0, b + 2, v > current_skills[n]);
	fire(change_skill, -1, n);
	caret.y += 2; height = 30;
	if(player->isclass(n))
		fore = fore.mix(colors::black, 224);
	else
		fore = fore.mix(colors::black, 128);
	caret.x = push.caret.x + 0; width = 160; texta(p->getname(), AlignLeftCenter);
	caret.x = push.caret.x + 169; width = 33; texta(str("%1i", v), AlignCenterCenter);
}

static featn get_next_feat(featn v) {
	while(v) {
		if(!player->basic.is(v))
			return v;
		auto n = bsdata<feati>::elements[v].upgrade;
		if(!n)
			break;
		v = n;
	}
	return (featn)0;
}

static featn get_feat(featn v) {
	while(v) {
		if(!player->basic.is(v))
			break;
		auto n = bsdata<feati>::elements[v].upgrade;
		if(!n)
			break;
		if(!player->basic.is(n))
			break;
		v = n;
	}
	return v;
}

static void paint_feat_row(void* object) {
	auto p = (feati*)object;
	auto bf = p->getindex();
	auto uf = get_next_feat(bf);
	auto pf = get_feat(bf);
	auto b = 3 * (bf % 3);
	auto allow_plus = feat_points > 0 && uf && !player->is(uf) && player->isallow(uf);
	auto allow_minus = pf && player->basic.is(pf) && !current_feats.is(pf);
	pushrect push; height = 28;
	caret.x = push.caret.x + 206;
	button(GBTNPLUS, b + 0, b + 1, 0, 0, b + 2, allow_plus);
	fire(change_feat, 1, uf);
	caret.x = push.caret.x + 224;
	button(GBTNMINS, b + 0, b + 1, 0, 0, b + 2, allow_minus);
	fire(change_feat, -1, pf);
	pushfore push_fore;
	if(player->isallow(bf))
		fore = fore.mix(colors::black, 224);
	else
		fore = fore.mix(colors::black, 128);
	caret.x = push.caret.x + 0; width = 160; texta(p->getname(), AlignLeftCenter);
	// Show all points possible
	caret.x += 190; caret.y += 12;
	while(true) {
		image(gres(GUIPFC), player->basic.is(bf) ? 0 : 1, 0);
		caret.x -= 12;
		bf = bsdata<feati>::elements[bf].upgrade;
		if(!bf)
			break;
	}
}

static void paint_choose_step() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
	paint_answers();
	paint_footer_answer(allow_next_button());
	paint_description();
	audio_update_channels();
}

static void paint_choose_sex() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
	image(254, 87, gres(GUIACG), 0, 0);
	setdialog(274, 271);
	paint_answers(196);
	paint_footer_answer(allow_next_button());
	paint_description();
	audio_update_channels();
}

static void paint_choose_abilities() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
	paint_dialog(254, 61, paint_choose_ability);
	paint_footer_answer(allow_next_button());
	paint_description();
	audio_update_channels();
}

static void paint_choose_skills() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
	static int origin;
	const int per_page = 10;
	auto skill_points = player->basic.abilities[SkillPoints];
	image(254, 87, gres(GUIACG), 3, 0);
	setdialog(254 + 27, 61 + 55, 205, 28); texta(getnm("SkillPointsLeft"), AlignCenterCenter);
	setdialog(254 + 241, 61 + 55, 33, 28); texta(str("%1i", skill_points), AlignCenterCenter);
	setdialog(254 + 31, 61 + 90, 254, 360);
	paint_list(records.data, records.element_size, records.count, origin, per_page,
		paint_skill_row, 36, {-9, 3}, -6, 0, 0, true);
	paint_footer_answer(skill_points <= 1);
	paint_description();
	audio_update_channels();
}

static void paint_choose_feats() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation("Feats");
	static int origin;
	const int per_page = 10;
	image(254, 87, gres(GUIACG), 4, 0);
	setdialog(254 + 27, 61 + 55, 205, 28); texta(getnm("FeatPointsLeft"), AlignCenterCenter);
	setdialog(254 + 241, 61 + 55, 33, 28); texta(str("%1i", feat_points), AlignCenterCenter);
	setdialog(254 + 31, 61 + 90, 254, 360);
	paint_list(records.data, records.element_size, records.count, origin, per_page,
		paint_feat_row, 36, {-9, 3}, -6, 0, 0, true);
	paint_footer_answer(feat_points == 0);
	paint_description();
	audio_update_channels();
}

static void paint_main_menu() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation("MainMenu");
	paint_steps();
	paint_footer();
	paint_description();
	audio_update_channels();
}

static void add_answer(nameable* p) {
	an.add(p, p->getname());
}

static void select_alignment() {
	auto& ei = bsdata<classi>::elements[player->getmainclass()];
	an.clear();
	for(auto& e : bsdata<alignmenti>()) {
		if(ei.is(e.getindex()))
			add_answer(&e);
	}
}

static void select_skills() {
	records.clear();
	for(auto& e : bsdata<skilli>()) {
		records.add(&e);
	}
	for(auto& e : bsdata<skilli>()) {
		auto n = e.getindex();
		current_skills[n] = player->basic.skills[n];
	}
	records.sort(compare_nameable);
}

static void select_feats(featgf v) {
	records.clear();
	for(auto& e : bsdata<feati>()) {
		if(e.is(UpgradeFeat))
			continue;
		if(!e.is(v))
			continue;
		records.add(&e);
	}
	records.sort(compare_nameable);
}

static int choose_avatar() {
	current_value = 0xFFFFFFFF;
	select_portrait(player->gender, player->portrait);
	if(current_value == 0xFFFFFFFF)
		random_avatar();
	if(!scene(paint_choose_avatar))
		return 0;
	return portraits[current_value]->getindex();
}

static void prepare_ability_normal() {
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
		ability_normal[i] = player->basic.abilities[i];
}

static bool open_choose_feat(featgf v, int count) {
	current_feats = player->basic.feats;
	feat_points = count;
	select_feats(v);
	if(!scene(paint_choose_feats)) {
		player->basic.feats = current_feats;
		return false;
	}
	return true;
}

static bool choose_step_action() {
	an.clear();
	set_description_id(bsdata<commandi>::elements[current_step].id);
	current_answer = 0;
	switch(current_step) {
	case ChooseGender:
		add_answer(bsdata<genderi>::elements + Male);
		add_answer(bsdata<genderi>::elements + Female);
		if(!scene(paint_choose_sex))
			return false;
		player->gender = (gendern)bsdata<genderi>::source.indexof(current_answer);
		player->portrait = choose_avatar();
		break;
	case ChooseRace:
		for(auto& e : bsdata<racei>())
			add_answer(&e);
		if(!scene(paint_choose_step))
			return false;
		player->race = (racen)bsdata<racei>::source.indexof(current_answer);
		for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1))
			player->basic.abilities[i] = 8;
		raise_race();
		break;
	case ChooseClass:
		for(auto i = 0; i < lenghtof(player->classes); i++)
			player->classes[i] = 0;
		for(auto& e : bsdata<classi>()) {
			if(e.player)
				add_answer(&e);
		}
		if(!scene(paint_choose_step))
			return false;
		raise_class((classn)bsdata<classi>::source.indexof(current_answer));
		break;
	case ChooseAlignment:
		select_alignment();
		if(!scene(paint_choose_step))
			return false;
		player->alignment = (alignmentn)bsdata<alignmenti>::source.indexof(current_answer);
		break;
	case ChooseAbilities:
		prepare_ability_normal();
		if(!scene(paint_choose_abilities))
			return false;
		break;
	case ChooseSkills:
		player->basic.abilities[SkillPoints] = skill_points_per_level(player->getmainclass()) * 4;
		if(player->basic.is(BonusSkills))
			player->basic.abilities[SkillPoints] += 4;
		select_skills();
		if(!scene(paint_choose_skills))
			return false;
		if(!open_choose_feat(GeneralFeat, player->basic.is(BonusFeat) ? 2 : 1))
			return false;
		break;
	}
	return true;
}

static void generate_step_by_step() {
	while(true) {
		set_step_description();
		auto p = scene(paint_main_menu);
		if(!p)
			break;
		if(bsdata<commandi>::have(p)) {
			current_step = (commandn)bsdata<commandi>::source.indexof(p);
			if(choose_step_action())
				current_step = (commandn)(current_step + 1);
		}
	}
}

void open_character_generation() {
	auto push_player = player;
	creature copy = {}; player = &copy;
#ifdef _DEBUG
	player->gender = Female;
	player->race = Human;
	raise_race();
	player->portrait = 14;
	raise_class(Fighter);
	player->alignment = (alignmentn)0;
	player->basic.abilities[Strenght] = 15;
	player->basic.abilities[Dexterity] = 11;
	player->basic.abilities[Constitution] = 10;
	player->basic.abilities[Intelligence] = 11;
	player->basic.abilities[Wisdow] = 12;
	player->basic.abilities[Charisma] = 10;
	current_step = ChooseSkills;
	//current_step = ChooseGender;
#else
	current_step = ChooseGender;
#endif // _DEBUG
	generate_step_by_step();
	player = push_player;
}