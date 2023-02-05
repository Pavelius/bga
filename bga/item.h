#include "crt.h"
#include "dice.h"
#include "magic.h"
#include "range.h"
#include "resid.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct animationi {
	const char*		avatar;
	const char*		ground;
	res::token		wear, thrown;
};
struct weaponi {
	char			bonus, ac, magic;
	dice			damage;
	range_s			range;
};
struct itemi : nameable {
	const char*		id_unidentified;
	magic_s			rate;
	unsigned		count, weight, cost;
	variants		dress, use;
	animationi		animation;
	weaponi			weapon;
	wear_s			wear;
};
struct item {
	unsigned short	type;
	unsigned char	count;
	union {
		struct {
			unsigned char broken : 2;
			unsigned char identified : 1;
			unsigned char charges : 5;
		};
		unsigned char data;
	};
	constexpr item() : type(0), count(0), data(0) {}
	constexpr explicit operator bool() { return type != 0; }
	void			add(item& v);
	bool			canequip(wear_s v) const;
	void			clear() { type = 0; count = 0; data = 0; }
	int				getcost() const { return geti().cost; }
	int				getcostall() const;
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
};
