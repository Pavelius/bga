#include "ability.h"
#include "item.h"
#include "stringbuilder.h"

static void addv(stringbuilder& sb, const char* id, const char* value) {
	sb.addn("%1: %2", getnm(id), value);
}

static void addb(stringbuilder& sb, const char* id, int value) {
	sb.addn("%1: %+2i", getnm(id), value);
}

void status_info() {
}

static void add_description(stringbuilder& sb, const char* id) {
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn(pn);
	sb.add("\n");
}

void item::getinfo(stringbuilder& sb) const {
	auto& ei = geti();
	add_description(sb, ei.id);
	if(ei.magic)
		addb(sb, "MagicBonus", ei.magic);
	if(ei.weight)
		addv(sb, "Weight", str("%1i pounds", ei.weight));
}