#include "alignment.h"
#include "answers.h"
#include "audio.h"
#include "command.h"
#include "creature.h"
#include "draw.h"
#include "portrait.h"
#include "rand.h"
#include "vector.h"
#include "view.h"

using namespace draw;

static vector<portraiti*> portraits;
static unsigned current_value;

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

static void paint_answers_sex() {
	image(254, 87, gres(GUISEX), 0, 0);
	setdialog(274, 271);
	paint_answers(196);
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

static void paint_footer_answer() {
	setdialog(35, 550); button(GBTNSTD, 1, 2, 0, "Biography", 3, false);
	setdialog(204, 550); button(GBTNSTD, 5, 6, 0, "Import", 7, false);
	setdialog(342, 550); button(GBTNSTD, 9, 10, KeyEscape, "Back"); fire(buttoncancel);
	setdialog(478, 550); button(GBTNSTD, 1, 2, KeyEnter, "Next", 3, current_answer != 0); fire(buttonok);
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
	image(254, 87, gres(GUISEX), 1, 0);
	image(295, 116, gres(PORTL), portraits[current_value]->getindex(), 0);
	setdialog(262, 284); button(GBTNPOR, 0, 1, KeyLeft); fire(cbsetint, current_value - 1, 0, &current_value);
	setdialog(512, 284); button(GBTNPOR, 2, 3, KeyRight); fire(cbsetint, current_value + 1, 0, &current_value);
	setdialog(276, 394 + 87); button(GBTNLRG, 1, 2, 'R', "Random"); fire(random_avatar);
	paint_footer_answer();
	paint_description();
	audio_update_channels();
}

static void paint_choose_ability() {
	auto ability_spent = 0;
	auto ability_left = 18 - ability_spent;
	image(254, 87, gres(GUISEX), 2, 0);
	setdialog(27, 57, 205, 28); texta(getnm("AbilityPointsLeft"), AlignCenterCenter);
	setdialog(240, 57, 33, 28); texta(str("%1i", ability_left), AlignCenterCenter);
	setdialog(24, 94, 120, 28);
	for(auto i = Strenght; i <= Charisma; i = (abilityn)(i + 1)) {
		auto push_caret = caret; width = 120; height = 28;
		texta(bsdata<abilityi>::elements[i].getname(), AlignRightCenter);
		caret.x = push_caret.x + 136; width = 33;
		texta(str("%1i", player->abilities[i]), AlignCenterCenter);
		caret.x = push_caret.x + 176;
		texta(str("%1i", player->abilities[i] / 2 - 5), AlignCenterCenter);
		auto b = 3 * ((i - Strenght) % 4);
		caret.y -= 3;
		caret.x = push_caret.x + 215; button(GBTNPLUS, b, b + 1);
		caret.x = push_caret.x + 233; button(GBTNMINS, b, b + 1);
		caret = push_caret;
		caret.y += 36;
	}
}

static void paint_choose_step() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
	switch(current_step) {
	case ChooseGender: paint_answers_sex(); break;
	case ChooseAbilities: paint_dialog(254, 61, paint_choose_ability); break;
	default: paint_answers(); break;
	}
	paint_footer_answer();
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

static void prepare_answers() {
	an.clear();
	switch(current_step) {
	case ChooseGender:
		add_answer(bsdata<genderi>::elements + Male);
		add_answer(bsdata<genderi>::elements + Female);
		break;
	case ChooseRace:
		for(auto& e : bsdata<racei>())
			add_answer(&e);
		break;
	case ChooseClass:
		for(auto i = 0; i < lenghtof(player->classes); i++)
			player->classes[i] = 0;
		for(auto& e : bsdata<classi>()) {
			if(e.player)
				add_answer(&e);
		}
		break;
	case ChooseAlignment:
		select_alignment();
		break;
	}
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

static bool choose_step_action() {
	set_description_id(bsdata<commandi>::elements[current_step].id);
	current_answer = 0;
	prepare_answers();
	if(!scene(paint_choose_step))
		return false;
	if(bsdata<genderi>::have(current_answer)) {
		player->gender = (gendern)bsdata<genderi>::source.indexof(current_answer);
		player->portrait = choose_avatar();
	} else if(bsdata<racei>::have(current_answer)) {
		player->race = (racen)bsdata<racei>::source.indexof(current_answer);
	} else if(bsdata<classi>::have(current_answer)) {
		player->classes[bsdata<classi>::source.indexof(current_answer)] = 1;
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
	current_step = ChooseAbilities;
	player->gender = Female;
	player->race = Human;
	player->portrait = 14;
	player->classes[Paladin] = 1;
	player->alignment = (alignmentn)0;
#else
	current_step = ChooseGender;
#endif // _DEBUG
	generate_step_by_step();
	player = push_player;
}