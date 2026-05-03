#include "ambient.h"
#include "answers.h"
#include "audio.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "formation.h"
#include "game.h"
#include "pushvalue.h"
#include "screenshoot.h"
#include "timer.h"
#include "list.h"
#include "math.h"
#include "variant.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

static void portrait_small() {
	if(!player)
		return;
	image(pma_port[0], player->portrait, 0);
}

static void view_chat_area() {
	paint_game_dialog(0, 316, "GUWBTP", 2);
	setdialog(31, 25); portrait_small();
	setdialog(706, 26, 76, 30); texta(str("%1i gp", player->coins), AlignRightBottom);
	setdialog(120, 20, 536, 204); paint_description(21, -11, 20);
	if(!an) {
		setdialog(120 + (536 - 205) / 2, 254);
		button(pma_butlrg2, 1, 2, KeyEnter, "Next");
		fire(buttoncancel);
	}
}

static void view_chat_dialog() {
	update_tick();
	audio_update_channels();
	update_area_music();
	update_ambients(camera);
	setcaret(0, 0, 800, 316);
	paint_area_no_command();
	view_chat_area();
}

void* choose_answers() {
	return scene(view_chat_dialog);
}

static void check_text(const char* value, unsigned key) {
	pushrect push;
	textfs(value);
	button_check(key);
}

static unsigned answers_hotkey(int index) {
	static char keys[] = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F'
	};
	if(index < lenghtof(keys))
		return keys[index];
	return 0;
}

void widget_answers() {
	pushfore push_fore;
	caret.x += 32; width -= 32;
	auto color_normal = colors::red.mix(colors::text, 64);
	auto color_hilite = colors::red.mix(colors::text);
	auto color_pressed = colors::red.mix(colors::text, 192);
	auto push_caret = caret;
	auto push_width = width;
	auto index = 0;
	for(auto& e : an) {
		caret.x = push_caret.x;
		width = push_width;
		fore = color_normal;
		text(str("%1i)", index + 1));
		caret.x += 24; width -= 24;
		check_text(e.text, answers_hotkey(index));
		if(button_pressed)
			fore = color_pressed;
		else if(button_hilited)
			fore = color_hilite;
		textf(e.text);
		if(button_executed)
			execute(buttonparam, (long)e.value);
		index++;
	}
}