#pragma once

#include "nameable.h"
#include "spell.h"
#include "flagable.h"

enum domainf : unsigned char {
	BonusPerDay, BonusPower,
};
struct domainfi : nameable {
};
typedef flagable<1, unsigned> domainfa;
struct domaini : nameable {
	spelli*		power;
	char		power_use_per_day;
	spelli*		spells[10]; // For each level one spell
	domainfa	tags;
	bool		is(domainf v) const { return tags.is(v); }
};
