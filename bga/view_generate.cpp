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

static commandn current_step;
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

static void start_over() {
	player->clear();
	current_step = ChooseGender;
	set_description(getnm("GenerationMainInfo"));
}

static void back_one_step() {
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

static void paint_choose_step() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
	if(current_step == ChooseGender)
		paint_answers_sex();
	else
		paint_answers();
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
	}
	return true;
}

static void generate_step_by_step() {
	while(true) {
		if(current_step == ChooseGender)
			set_description(getnm("GenerationMainInfo"));
		else
			set_description("%CharacterBriefInfo");
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
	current_step = ChooseGender;
	generate_step_by_step();
	player = push_player;
}