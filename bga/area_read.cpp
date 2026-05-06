#include "area.h"
#include "bsreq.h"
#include "creaturea.h"
#include "container.h"
#include "itemground.h"
#include "log.h"
#include "logvalue.h"
#include "modifier.h"
#include "npc.h"
#include "pushvalue.h"
#include "script.h"
#include "variant.h"

using namespace log;

typedef const char* (*fnstrparse)(const char* p);

static char temp[512];

static bool equalid(const char* p, const char* s) {
	if(!p || !s)
		return false;
	while(*s && *p)
		if(*p++ != *s++)
			return false;
	return !isnum(*p) && p[0] != '_' && !ischa(*p);
}

static const char* skip(const char* p, char v) {
	if(p[0] != v) {
		char symbol[2] = {v, 0};
		errorp(p, "Expected `%1`", symbol);
		return skipline(p);
	}
	return skipws(p + 1);
}

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

static const char* psidf(const char* p) {
	stringbuilder sb(temp); sb.clear();
	return psidf(p, sb);
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

static const char* read_array(const char* p, unsigned char* values, int maximum) {
	auto index = 0;
	while(isnum(*p) && index < maximum) {
		int value = 0;
		p = psnum(p, value);
		p = skipws(p);
		values[index++] = (unsigned char)value;
	}
	// Zero fill
	while(index < maximum)
		values[index++] = 0;
	return p;
}

static const char* psvalue(const char* p, const char* id, fnstrparse proc) {
	if(equalid(p, id)) {
		auto n = zlen(id);
		if(p[n] != '(')
			return p;
		p = skipws(p + n + 1);
		p = proc(p);
		p = skip(p, ')');
	}
	return p;
}

static const char* read_colors(const char* p) {
	return read_array(p, player->colors, lenghtof(player->colors));
}

static const char* create_creature(const char* p) {
	player = bsdata<creature>::addz();
	player->clear();
	player->gender = Male;
	player->portrait = 0xFFFF;
	p = psnum(p, player->position_index);
	p = psnum(p, player->orientation);
	player->position = i2sc(player->position_index);
	while(*p) {
		auto p1 = p;
		p = psvalue(p, "colors", read_colors);
		if(p1 != p)
			continue;
		break;
	}
	p = skipws(psidf(p));
	pushvalue push(modifier, Permanent);
	auto pm = bsdata<npci>::find(temp);
	if(pm) {
		set_player_name(temp);
		ftscript<npci>(pm - bsdata<npci>::elements, 0);
	} else {
		create_abilities(true);
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
	pushvalue push(modifier, InsideContainer);
	p = read_variants(p);
	return p;
}

static const char* create_item(const char* p) {
	short unsigned index = 0;
	p = psnum(p, index);
	p = skipws(psidf(p));
	auto pi = bsdata<itemi>::find(temp);
	if(!pi) {
		log::errorp(p, "Not found item description `%1`", temp);
		pi = bsdata<itemi>::elements;
	}
	item it(pi - bsdata<itemi>::elements);
	add_item(current_area, i2sc(index), it);
	p = read_variants(p);
	return p;
}

static const char* read_block(const char* p) {
	p = skipws(psidf(p));
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