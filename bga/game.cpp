#include "area.h"
#include "console.h"
#include "entrance.h"
#include "draw_control.h"
#include "game.h"
#include "timer.h"

unsigned getgamehour() {
	// In game you have 1 second = 1 minute. So 60 seconds is hour.
	return (current_game_tick / (1000 * 60)) % 24;
}

static void read_area(const char* id, const char* folder) {
	auto p = areai::add(id, folder);
	current_area = getbsi(p);
}

void enter(const char* id, const char* location) {
	char temp[32]; stringbuilder sb(temp); sb.add(location);
	logm("Enter area [%1] at location [%2]", id, location);
	read_area(id, "ID2");
	map::read(id);
	auto pn = entrance::find(temp);
	if(pn)
		setcamera(pn->position);
	draw::form::nextscene("GGAME");
}