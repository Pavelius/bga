#include "bsdata.h"
#include "creature.h"
#include "spell.h"

spellbook* last_spellbook;

void spellbook::clear() {
	memset(this, 0, sizeof(*this));
}

const char* spellbook::getid() const {
	switch(type) {
	case DomainClass: return "Domain";
	default: return bsdata<classi>::elements[type].id;
	}
}

spelln spelli::getindex() const {
	return (spelln)(this - bsdata<spelli>::elements);
}

spellbook* find_spellbook(short unsigned owner, short unsigned type) {
	for(auto& e : bsdata<spellbook>()) {
		if(e.owner == owner && e.type == type)
			return &e;
	}
	return 0;
}

spellbook* get_spellbook(creature* player, short unsigned type) {
	auto i = player->getindex();
	auto p = find_spellbook(i, type);
	if(!p) {
		p = bsdata<spellbook>::add();
		p->clear();
		p->owner = i;
		p->type = type;
	}
	return p;
}

void add_player_spellbooks() {
	if(!player->isparty())
		return;
	for(auto& e : bsdata<classi>()) {
		if(!e.cast)
			continue;
		auto n = e.getindex();
		if(!player->classes[n])
			continue;
		get_spellbook(player, n);
		if(n == Cleric) {
			get_spellbook(player, DomainClass);
			add_known_spells(n, 1);
		}
	}
}

void add_known_spells(classn type, int level) {
	auto p = find_spellbook(player->getindex(), type);
	if(!p)
		return;
	for(auto& e : bsdata<spelli>()) {
		if(e.classes[type] == level)
			p->known.set(e.getindex());
	}
}