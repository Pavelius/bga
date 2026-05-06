#include "ability.h"
#include "area.h"
#include "audio.h"
#include "bsreq.h"
#include "cflags.h"
#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "itema.h"
#include "keybind.h"
#include "log.h"
#include "option.h"
#include "party.h"
#include "rand.h"
#include "rfiles.h"
#include "script.h"
#include "spell.h"
#include "talk.h"
#include "timer.h"
#include "view.h"
#include "worldmap.h"

using namespace draw;

void main_identifier(stringbuilder& sb, const char* identifier);
void initialize_translation();
void util_main();

int main(int argc, char* argv[]) {
	srand(getcputime());
	// srand(923811);
	stringbuilder::custom = main_identifier;
	initialize_resources();
	// initialize_data(bsdata<talki>::source, "locale/%1", "*.tlk");
	bsreq::read("rules/Basic.txt");
	read_options();
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	audio_allow_music = false;
	initialize_items();
	initialize_translation();
	initialize_colorgrad();
	initialize_feats();
	initialize_parties();
	initialize_ui();
	initialize_interface();
	initialize_keybind();
	if(log::errors)
		return -1;
	metrics::font = gres("NORMAL");
	metrics::h1 = gres("STONEBIG");
	metrics::h2 = gres("REALMS");
	metrics::h3 = gres("NORMAL");
	metrics::small = gres("TOOLFONT");
	colors::text = color(255, 255, 255);
	colors::h3 = color(244, 214, 66);
	colors::h1 = colors::text.mix(colors::h3, 64);
	colors::h2 = colors::text.mix(colors::h3, 96);
	colors::special = color(244, 214, 66);
	update_area_music();
	initialize(getnm("AppTitle"));
	sys_cursor(false);
	sys_timer(64);
	//next_scene(start_main);
	next_scene(open_main_menu);
	start_scene();
	return 0;
}

#ifndef __GNUC__
int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}
#endif // __GNUC__