#include "area.h"
#include "console.h"
#include "door.h"
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
	current_variable_base = p->variables.begin();
}

static void use_all_doors() {
	for(auto& e : bsdata<door>())
		e.use(e.isopen());
}

void enter(const char* id, const char* location) {
	char temp[32]; stringbuilder sb(temp); sb.add(location);
	logm("Enter area [%1] at location [%2]", id, location);
	map::read(id);
	read_area(id, "ID2");
	use_all_doors();
	auto pn = entrance::find(temp);
	if(pn)
		setcamera(pn->position);
	draw::form::nextscene("GGAME");
}