#include "area.h"
#include "archive.h"
#include "console.h"
#include "creature.h"
#include "door.h"
#include "draw.h"
#include "entrance.h"
#include "formation.h"
#include "game.h"
#include "itemground.h"
#include "iteminside.h"
#include "region.h"
#include "saveheader.h"
#include "timer.h"
#include "view.h"

gamei game;

static void read_area(const char* id, const char* folder) {
	auto p = add_area(id, folder);
	current_area = getbsi(p);
	current_variable_base = p->variables.begin();
}

static void use_all_doors() {
	for(auto& e : bsdata<door>())
		e.use(e.isopen());
}

static point get_free(point dst, point src, formationn formation, int index, int size) {
	auto v = get_formation(dst, src, formation, index);
	return get_free(v, size);
}

void setparty(point dst) {
	auto index = 0;
	auto p = get_selected();
	if(!p)
		return;
	auto start_position = p->position;
	for(auto p : party_selected) {
		if(!p)
			continue;
		auto new_position = get_free(dst, start_position, FormationProtect, index++, p->getsize());
		p->area_index = current_area;
		p->lookat(new_position);
		p->setposition(new_position);
	}
}

void moveparty(point dst) {
	auto index = 0;
	auto p = get_selected();
	if(!p)
		return;
	auto start_position = p->position;
	for(auto p : party_selected) {
		if(!p)
			continue;
		p->moveto(get_free(dst, start_position, FormationProtect, index++, p->getsize()));
	}
}

void enter(const char* id, const char* location) {
	char temp[32]; stringbuilder sb(temp); sb.add(location);
	print("Enter area [%1] at location [%2]", id, location);
	read_area(id);
	read_area(id, "ID2");
#ifdef _DEBUG
	print("Count of points %1i", bsdata<point>::source.count);
	print("Count of doors %1i", bsdata<door>::source.count);
	print("Count of regions %1i", bsdata<region>::source.count);
#endif
	use_all_doors();
	auto pn = entrance::find(temp);
	if(pn) {
		setcamera(pn->position);
		setparty(pn->position);
	}
	next_scene(open_game);
}

static void serial_header(archive& a, saveheaderi& v) {
	if(a.writemode) {
		v.create();
		a.set(v);
	} else
		a.set(v);
}

static unsigned long get_version() {
	unsigned long i = 0;
	unsigned long r = sizeof(gamei)*(++i);
	r += sizeof(areai) * (++i);
	r += sizeof(creature) * (++i);
	r += sizeof(itemground) * (++i);
	r += sizeof(iteminside) * (++i);
	r += sizeof(variable) * (++i);
	return r;
}

bool saveheaderi::read(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return false;
	archive a(file, false);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(get_version()))
		return false;
	serial_header(a, *this);
	return true;
}

static bool archive_sav(const char* url, bool write_mode) {
	io::file file(url, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(get_version()))
		return false;
	saveheaderi* phead = new saveheaderi;
	serial_header(a, *phead);
	delete phead;
	a.set(area_name);
	a.set(draw::camera);
	a.set(current_game_tick);
	a.set(wearable::coins);
	a.set(bsdata<areai>::source);
	a.set(bsdata<variable>::source);
	a.set(bsdata<creature>::source);
	a.set(bsdata<itemground>::source);
	a.set(bsdata<iteminside>::source);
	return true;
}

static bool archive_game(const char* name, bool write_mode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("save/%1.sav", name);
	return archive_sav(temp, write_mode);
}

void gamesave(const char* name) {
	archive_game(name, true);
}

void create_game() {
	game.set(IdentifyCost, 100);
}