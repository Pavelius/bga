#include "area.h"
#include "archive.h"
#include "console.h"
#include "creature.h"
#include "door.h"
#include "draw_form.h"
#include "entrance.h"
#include "formation.h"
#include "game.h"
#include "timer.h"

unsigned getgamehour() {
	// In game you have 1 second = 1 minute. So 60 seconds is hour.
	return (current_game_tick / (1000 * 60)) % 24;
}

static void read_area(const char* id, const char* folder) {
	auto p = areai::add(id, folder);
	current_area = getbsi(p);
	current_variable_base = p->variables.begin();
}

static void use_all_doors() {
	for(auto& e : bsdata<door>())
		e.use(e.isopen());
}

point getformationfree(point dst, point src, formation_s formation, int index) {
	return map::getfree(getformation(dst, src, formation, index), 1);
}

void setparty(point dst) {
	auto index = 0;
	if(!selected_creatures)
		return;
	auto start_position = selected_creatures[0]->position;
	for(auto p : selected_creatures) {
		auto new_position = getformationfree(dst, start_position, FormationProtect, index++);
		p->area_index = current_area;
		p->lookat(new_position);
		p->setposition(new_position);
	}
}

void enter(const char* id, const char* location) {
	char temp[32]; stringbuilder sb(temp); sb.add(location);
	logm("Enter area [%1] at location [%2]", id, location);
	map::read(id);
	read_area(id, "ID2");
	use_all_doors();
	auto pn = entrance::find(temp);
	if(pn) {
		setcamera(pn->position);
		setparty(pn->position);
	}
	draw::form::nextscene("GGAME");
}

static bool archive_sav(const char* url, bool write_mode) {
	io::file file(url, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	a.set(bsdata<variable>::source);
	a.set(bsdata<creature>::source);
	return true;
}

static bool archive_game(const char* name, bool write_mode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("save/%1.sav", name);
	return archive_sav(temp, write_mode);
}