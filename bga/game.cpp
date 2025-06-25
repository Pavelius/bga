#include "ambient.h"
#include "audio.h"
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
#include "playlist.h"
#include "rand.h"
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
		if(orientation==0xFF)
			p->lookat(new_position);
		else
			p->orientation = orientation;
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
		p->moveto(get_free(dst, start_position, current_formation, index++, p->getsize()));
	}
}

void enter(const char* id, const char* location) {
	char entrance_copy[32]; stringbuilder sb(entrance_copy); sb.add(location);
	print("Enter area [%1] at location [%2]", id, location);
	audio_reset();
	read_area(id, "ID2");
	read_area(id);
	playlist_play(id, "ID2", PlayDay);
#ifdef _DEBUG
	print("Count of points %1i", bsdata<point>::source.count);
	print("Count of doors %1i", bsdata<door>::source.count);
	print("Count of regions %1i", bsdata<region>::source.count);
#endif
	use_all_doors();
	auto pn = entrance::find(entrance_copy);
	if(pn) {
		setcamera(pn->position);
		setparty(pn->position, pn->orientation);
	}
	initialize_area_ambients();
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

bool is_saved_game(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return false;
	archive a(file, false);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(get_version()))
		return false;
	return true;
}

const char* get_save_url(char* result, const char* id) {
	stringbuilder sb(result, result + 259);
	sb.clear();
	sb.add("save/%1.sav", id);
	return result;
}

bool rowsaveheaderi::read() {
	char temp[260];
	io::file flo(get_save_url(temp, file), StreamRead);
	if(!flo)
		return false;
	archive a(flo, false);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(get_version()))
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
	if(!a.signature(get_version()))
		return false;
	if(!write_mode)
		flo.get(change);
	serial_header(a, *this);
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

void game_auto_save() {
	auto p = new rowsaveheaderi;
	p->clear();
	p->setname(getnm("Autosave"));
	p->setfile("Autosave");
	p->serial(true);
	delete p;
}

void create_game() {
	game.set(IdentifyCost, 100);
	game.set(Rounds, xrand(10, 30));
}