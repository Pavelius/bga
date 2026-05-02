#include "action.h"
#include "ambient.h"
#include "audio.h"
#include "area.h"
#include "archive.h"
#include "console.h"
#include "container.h"
#include "creaturea.h"
#include "door.h"
#include "draw.h"
#include "entrance.h"
#include "formation.h"
#include "game.h"
#include "itemground.h"
#include "rand.h"
#include "region.h"
#include "saveheader.h"
#include "script.h"
#include "screenshoot.h"
#include "store.h"
#include "timer.h"
#include "view.h"
#include "worldmap.h"

#define AREA_DISAPEAR

using namespace draw;

gamei game;
int last_number;

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
	set(e.flags);
}

void debugmsg(const char* format, ...) {
#ifdef _DEBUG
	XVA_FORMAT(format);
	printcnv(format, format_param);
#endif
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
	player->order.clear();
	auto start_position = p->position;
	for(auto p : party_selected) {
		if(!p)
			continue;
		p->moveto(get_free(v, start_position, current_formation, index++, p->getsize()));
	}
}

static void load_area(areai* area) {
	need_update_creatures = true;
	audio_reset();
	read_area(area);
	use_all_doors();
	update_area_music();
	initialize_area_ambients();
	need_update_creatures = true;
}

void enter(const char* location) {
#ifdef AREA_DISAPEAR
	scene_disapear(0, colors::black);
#endif
	auto pn = bsdata<entrancei>::find(location);
	if(!pn)
		return;
	load_area(pn->area);
	debugmsg("Enter location [%1]", pn->id);
	setcamera(pn->position);
	setparty(pn->position, pn->orientation);
#ifdef AREA_DISAPEAR
	update_frames();
	scene_appear(view_game_area, 0);
#endif
	next_scene(open_game);
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

void game_disappear() {
	scene_disapear(0, colors::black);
}

void game_appear(bool need_next_scene) {
	update_frames();
	scene_appear(view_game_area, 0);
	if(need_next_scene)
		next_scene(open_game);
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
	a.set(game);
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
#ifdef AREA_DISAPEAR
		game_disappear();
#endif
		auto p = bsdata<areai>::find(area_name);
		if(!p)
			return false;
		load_area(p);
#ifdef AREA_DISAPEAR
		game_appear(false);
#endif
		next_scene(open_game);
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

static void create_game() {
	game.clear();
	game.set(IdentifyCost, 100);
	game.set(Rounds, xrand(10, 30));
}

void party_action(point target_position, actionn action, short unsigned target) {
	if(!player)
		return;
	if(!bsdata<actioni>::elements[action].proc)
		return;
	player->order.clear();
	auto position = player->position;
	if(distance(position, target_position) > 24) {
		player->moveto(target_position);
		player->order.target = target;
		player->order.action = action;
	} else
		execute(bsdata<actioni>::elements[action].proc, (long)player, target);
}

void gamei::clear() {
	memset(abilities, 0, sizeof(*this));
	bsdata<itemground>::source.clear();
}

void initialize_story() {
	create_game();
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

static void open_door() {
	auto p = getbs<door>((short unsigned)hot.param2);
	p->use(!p->isopen());
}

BSDATA(actioni) = {
	{"NoAction"},
	{"ActionDefend", 0 * 4},
	{"ActionTurn", 1 * 4},
	{"ActionCast", 2 * 4},
	{"ActionAttack", 3 * 4},
	{"ActionUseItem", 4 * 4},
	{"ActionInspiration", 5 * 4},
	{"ActionTheivery", 6 * 4},
	{"ActionHide", 7 * 4},
	{"ActionHead", 8 * 4},
	{"ActionSearch", 9 * 4},
	{"ActionSpecialAbility", 10 * 4},
	{"ActionStop", 11 * 4},
	{"ActionLeft", 12 * 4},
	{"ActionRight", 13 * 4},
	{"ActionPlayMusic", 14 * 4},
	{"ActionEntangle", 15 * 4},
	{"ActionWildernessLore", 16 * 4},
	{"ActionMeleeAttack", 17 * 4},
	{"ActionRangeAttack", 18 * 4},
	{"ActionOpenContainer", 0, open_container},
	{"ActionOpenDoor", 0, open_door},
	{"ActionPickItems", 0, open_ground_items},
};
assert_enum(actioni, ActionPickItems)