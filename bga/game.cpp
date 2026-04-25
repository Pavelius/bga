#include "ambient.h"
#include "audio.h"
#include "area.h"
#include "archive.h"
#include "console.h"
#include "container.h"
#include "creature.h"
#include "door.h"
#include "draw.h"
#include "entrance.h"
#include "formation.h"
#include "game.h"
#include "itemground.h"
#include "order.h"
#include "rand.h"
#include "region.h"
#include "saveheader.h"
#include "script.h"
#include "store.h"
#include "timer.h"
#include "view.h"
#include "worldmap.h"

using namespace draw;

gamei game;

template<> void archive::set<creature*>(creature*& v) {
	setpointer(bsdata<creature>::source, (void**)&v);
}

template<> void archive::set<worldmapi*>(worldmapi*& v) {
	setpointer(bsdata<worldmapi>::source, (void**)&v);
}

template<> void archive::set<areai>(areai& e) {
	set(e.doors_opened);
	set(e.doors_locked);
	set(e.doors_trapped);
	set(e.region_disabled);
	set(e.animate_disabled);
}

static void use_all_doors() {
	for(auto& e : bsdata<door>())
		e.use(e.isopen());
}

static point get_free(point dst, point src, formationn formation, int index, int size) {
	auto v = get_formation(dst, src, formation, index);
	return get_free(v, size);
}

void setparty(point dst, unsigned char orientation) {
	auto index = 0;
	auto p = get_selected();
	if(!p)
		return;
	auto start_position = p->position;
	for(auto p : party_selected) {
		if(!p)
			continue;
		auto new_position = get_free(dst, start_position, current_formation, index++, p->getsize());
		p->area_index = current_area;
		if(orientation == 0xFF)
			p->lookat(new_position);
		else
			p->orientation = orientation;
		p->setposition(new_position);
	}
}

void party_move(point v) {
	auto index = 0;
	auto p = get_selected();
	if(!p)
		return;
	clear_orders(player);
	auto start_position = p->position;
	for(auto p : party_selected) {
		if(!p)
			continue;
		p->moveto(get_free(v, start_position, current_formation, index++, p->getsize()));
	}
}

static void load_area(areai* area) {
	audio_reset();
	read_area(area);
	use_all_doors();
	update_area_music();
	initialize_area_ambients();
	next_scene(open_game);
}

void enter(const char* location) {
	auto pn = bsdata<entrancei>::find(location);
	if(!pn)
		return;
	load_area(pn->area);
#ifdef _DEBUG
	print("Enter location [%1]", pn->id);
#endif
	setcamera(pn->position);
	setparty(pn->position, pn->orientation);
}

void enter_from_wmap(const char* area) {
	char temp[16]; stringbuilder sb(temp);
	sb.add(area);
	sb.add("FRWMAP");
	sb.upper();
	enter(temp);
}

static unsigned long game_signature() {
	unsigned long i = 0;
	unsigned long r = sizeof(gamei) * (++i);
	r += sizeof(areai) * (++i);
	r += sizeof(creature) * (++i);
	r += sizeof(itemground) * (++i);
	r += bsdata<itemi>::source.count * (++i);
	r += bsdata<areai>::source.count * (++i);
	return r;
}

bool is_saved_game(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return false;
	archive a(file, false);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(game_signature()))
		return false;
	return true;
}

const char* get_save_url(char* result, const char* id) {
	stringbuilder sb(result, result + 259);
	sb.clear();
	sb.add("save/%1.sav", id);
	return result;
}

static void serial_header(archive& a, saveheaderi& v) {
	if(a.writemode) {
		v.create();
		a.set(v);
	} else
		a.set(v);
}

bool rowsaveheaderi::read() {
	char temp[260];
	io::file flo(get_save_url(temp, file), StreamRead);
	if(!flo)
		return false;
	archive a(flo, false);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(game_signature()))
		return false;
	flo.get(change);
	serial_header(a, *this);
	return true;
}

bool rowsaveheaderi::serial(bool write_mode) {
	char temp[260];
	io::file flo(get_save_url(temp, file), write_mode ? StreamWrite : StreamRead);
	if(!flo)
		return false;
	archive a(flo, write_mode);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(game_signature()))
		return false;
	if(!write_mode)
		flo.get(change);
	serial_header(a, *this);
	a.set(area_name);
	a.set(camera);
	a.set(current_game_tick);
	a.set(player);
	a.set(party);
	a.set(party_selected);
	a.set(game_panel_mode);
	a.set(current_world);
	a.set(wearable::coins);
	a.setc<areai>(bsdata<areai>::source);
	a.set(bsdata<creature>::source);
	a.set(bsdata<itemground>::source);
	if(!write_mode) {
		auto p = bsdata<areai>::find(area_name);
		if(!p)
			return false;
		load_area(p);
	}
	return true;
}

void game_auto_save() {
	auto p = new rowsaveheaderi;
	p->clear();
	p->setname(getnm("Autosave"));
	p->setfile("Autosave");
	p->serial(true);
	delete p;
}

static void game_quick_save(bool writemode) {
	auto p = new rowsaveheaderi;
	p->clear();
	p->setname(getnm("Quicksave"));
	p->setfile("Quicksave");
	p->serial(writemode);
	delete p;
}

void game_quick_save() {
	game_quick_save(true);
	print(getnm("GameQuickSaved"));
}

void game_quick_load() {
	game_quick_save(false);
}

void create_game() {
	game.set(IdentifyCost, 100);
	game.set(Rounds, xrand(10, 30));
}

point get_action_position(void* object, point nearest) {
	if(bsdata<region>::have(object)) {
		auto p = (region*)object;
		return p->use;
	} else if(bsdata<door>::have(object)) {
		auto p = (door*)object;
		auto d = distance(p->position, nearest);
		if(p->position_alternate && d > distance(p->position_alternate, nearest))
			return p->position_alternate;
		return p->position;
	} else if(bsdata<container>::have(object)) {
		auto p = (container*)object;
		return p->launch;
	} else if(bsdata<creature>::have(object)) {
		//if(combat_mode) {
		//} else
		//	execute(choose_creature, 0, 0, object);
	}
	return {0, 0};
}

void party_action(void* object, point target_position, fnevent apply) {
	if(!player)
		return;
	clear_orders(player);
	auto position = player->position;
	if(distance(position, target_position) > 24) {
		player->moveto(target_position);
		add_order(player, object, apply);
	} else
		execute(apply, 0, 0, object);
}

void initialize_story() {
	initialize_store();
	current_world = bsdata<worldmapi>::elements;
	player = party[0];
	script_run("StartGame");
}

int game_rand(int v1, int v2) {
	return xrand(v1, v2);
}

bool game_chance(int v) {
	return (rand() % 100) < v;
}