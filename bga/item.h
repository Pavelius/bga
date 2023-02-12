#include "crt.h"
#include "damage.h"
#include "dice.h"
#include "feat.h"
#include "magic.h"
#include "range.h"
#include "resid.h"
#include "variant.h"
#include "wear.h"

#pragma once

enum itemf_s : unsigned char {
	TwoHanded, Balanced, UltraBalanced, Dangerous, Cursed,
	BrilliantEnergy, Disruption, Distance, Flaming,
	FlamingBurst, Frost, GhostTouch, Holy, IcyBurst,
	Keen, Seeking, Shock, ShockingBurst, Speed,
	Unholy, Vicious, Vorpal, Wounding,
};
struct itemfi {
	const char*		id;
};
struct itemresi {
};
struct weaponi {
	damage_s		type;
	dice			damage;
	range_s			range;
	char			bonus;
};
struct itemi : nameable {
	unsigned		count, weight, cost;
	short			avatar, ground;
	res::token		equiped, thrown;
	weaponi			weapon;
	unsigned		flags;
	wear_s			wear;
	char			magic;
	variants		use, wearing;
	itemi*			basic;
	char			max_dex_bonus;
	feat_s			required[3];
	bool			is(itemf_s v) const { return (flags & (1 << v)) != 0; }
};
struct item {
	unsigned short	type;
	unsigned char	count;
	union {
		struct {
			unsigned char identified : 1;
			unsigned char personal : 1;
		};
		unsigned char data;
	};
	constexpr item() : type(0), count(0), data(0) {}
	item(unsigned short type) : type(type), count(geti().count ? geti().count - 1 : 0), data(0) {}
	constexpr explicit operator bool() { return type != 0; }
	void			add(item& v);
	bool			canequip(wear_s v) const;
	void			clear() { type = 0; count = 0; data = 0; }
	int				getcost() const { return geti().cost; }
	int				getcostall() const;
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
	void			getinfo(stringbuilder& sb) const;
	const char*		getname() const;
};
extern item* last_item;
