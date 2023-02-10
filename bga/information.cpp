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

void getinfo(stringbuilder& sb, item* pi) {
	auto& ei = pi->geti();
	if(ei.magic)
		addb(sb, "MagicBonus", ei.magic);
}