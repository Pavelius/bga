#include "crt.h"
#include "dice.h"
#include "magic.h"
#include "range.h"
#include "resid.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct itemresi {
};
struct weaponi {
	char			bonus, ac, magic;
	dice			damage;
	range_s			range;
};
struct itemi : nameable {
	unsigned		count, weight, cost;
	weaponi			weapon;
	variants		dress, use;
	short			avatar, ground;
	res::token		equiped, thrown;
	wear_s			wear;
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
	constexpr explicit operator bool() { return type != 0; }
	void			add(item& v);
	bool			canequip(wear_s v) const;
	void			clear() { type = 0; count = 0; data = 0; }
	int				getcost() const { return geti().cost; }
	int				getcostall() const;
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
};
