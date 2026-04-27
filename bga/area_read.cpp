#include "area.h"
#include "bsreq.h"
#include "creaturea.h"
#include "container.h"
#include "itemground.h"
#include "log.h"
#include "logvalue.h"
#include "npc.h"
#include "pushvalue.h"
#include "script.h"
#include "variant.h"

using namespace log;

static char temp[512];

static const char* psnum(const char* p, void* object, int size) {
	int number = 0;
	if(*p == '-' || *p == '+' || isnum(*p))
		p = psnum(p, number);
	else
		log::errorp(p, "Expected number");
	p = skipws(p);
	switch(size) {
	case 1: *((unsigned char*)object) = number; break;
	case 2: *((unsigned short*)object) = number; break;
	case 4: *((unsigned*)object) = number; break;
	default: break;
	}
	return p;
}

static const char* psnum(const char* p, unsigned short& value) {
	return psnum(p, &value, sizeof(value));
}

static const char* psnum(const char* p, unsigned char& value) {
	return psnum(p, &value, sizeof(value));
}

static void apply_custom(const char* id) {
}

static const char* read_variants(const char* p) {
	stringbuilder sb(temp);
	while(*p == '_' || ischa(*p)) {
		variant v = {};
		p = skipws(log::psval(p, v));
		script_run(v);
	}
	return p;
}

static void set_player_name(const char* id) {
	player->name.set(getnm(id));
}

static const char* create_creature(const char* p) {
	stringbuilder sb(temp); sb.clear();
	player = bsdata<creature>::add();
	player->clear();
	player->area_index = current_area;
	player->portrait = 0xFFFF;
	p = psnum(p, player->position_index);
	p = psnum(p, player->orientation);
	player->position = i2sc(player->position_index);
	p = skipws(psidf(p, sb));
	auto pm = bsdata<npci>::find(temp);
	if(pm) {
		set_player_name(temp);
		ftscript<npci>(pm - bsdata<npci>::elements, 0);
	} else {
		set_player_name(temp);
		player->feats.set(DynamicAnimation);
	}
	p = read_variants(p);
	player_finish();
	need_update_creatures = true;
	return p;
}

static const char* create_container(const char* p) {
	short unsigned index = 0;
	p = psnum(p, index);
	last_container = bsdata<container>::elements + index;
	p = read_variants(p);
	return p;
}

static const char* create_item(const char* p) {
	short unsigned index = 0;
	p = psnum(p, index);
	auto pi = bsdata<itemground>::addz();
	pi->area = current_area;
	pi->position = i2sc(index);
	p = read_variants(p);
	return p;
}

static const char* read_block(const char* p) {
	stringbuilder sb(temp);
	p = skipws(psidf(p, sb));
	if(equal(temp, "Creature"))
		return create_creature(p);
	else if(equal(temp, "Container"))
		return create_container(p);
	else if(equal(temp, "Item"))
		return create_item(p);
	return p;
}

void area_read(const char* url) {
	pushvalue push_player(player);
	pushvalue push_container(last_container);
	pushvalue push(log::context);
	auto p = log::read(url);
	if(!p)
		return;
	p = skipwscr(p);
	while(*p) {
		p = read_block(p);
		p = skipwscr(skipline(p));
	}
	log::close();
}