#include "answers.h"
#include "audio.h"
#include "command.h"
#include "creature.h"
#include "draw.h"
#include "view.h"

using namespace draw;

static commandn current_step;
static void* current_answer;

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
	set_description_id(p->id);
	current_answer = p;
}

static void paint_answers() {
	setdialog(274, 113);
	for(auto& e : an) {
		auto i = an.indexof(&e);
		auto b = (i % 4) * 4;
		auto c = b + 1;
		if(current_answer == e.value)
			c = b + 0;
		button(GBTNLRG, c, b + 2, '1' + i, e.text, b + 3, true, false);
		fire(pick_answer, 0, 0, e.value);
		caret.y += 35;
	}
}

static void start_over() {
	player->clear();
	current_step = ChooseGender;
}

static void paint_footer() {
	setdialog(35, 550); button(GBTNSTD, 1, 2, 0, "Biography", 3, false);
	setdialog(204, 550); button(GBTNSTD, 5, 6, 0, "Import");
	setdialog(342, 550); button(GBTNSTD, 9, 10, KeyEscape, "Back", 11, false); fire(buttoncancel);
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
	paint_description(9, -2, 4);
}

static void paint_choose_step() {
	paint_game_dialog(GUICGB);
	paint_portrait();
	paint_character_generation(bsdata<commandi>::elements[current_step].id);
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

static void choose_step_action() {
	set_description_id(bsdata<commandi>::elements[current_step].id);
	current_answer = 0;
	prepare_answers();
	auto p = scene(paint_choose_step);
}

static void generate_step_by_step() {
	while(true) {
		set_description(getnm(ids(bsdata<commandi>::elements[current_step].id, "Main")));
		auto p = scene(paint_main_menu);
		if(!p)
			break;
		if(bsdata<commandi>::have(p)) {
			current_step = (commandn)bsdata<commandi>::source.indexof(p);
			choose_step_action();
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