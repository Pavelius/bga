#include "colorgrad.h"
#include "draw.h"
#include "draw_command.h"
#include "script.h"

using namespace draw;

static void test(int bonus) {
}

BSDATA(script) = {
	{"Test", test}
};
BSDATAF(script)

static void color_pick() {
	auto color_index = color_indecies[hot.param];
	if(color_index == -1)
		return;
	breakmodal(color_index);
}

static void default_color_pick() {
	breakmodal(default_color);
}

BSDATA(draw::command) = {
	{"Cancel", draw::buttoncancel, KeyEscape},
	{"ColorPick", color_pick},
	{"DefaultColor", default_color_pick, KeyEscape},
	{"Done", draw::buttonok, KeyEnter},
};
BSDATAF(draw::command)