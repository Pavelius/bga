#include "ability.h"
#include "dice.h"
#include "magic.h"
#include "range.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct rfpma;

enum featn : unsigned char;
enum itemf : unsigned char {
	TwoHanded, Balanced, UltraBalanced, Dangerous, Cursed,
	BrilliantEnergy, Disruption, Distance, Flaming,
	FlamingBurst, Frost, GhostTouch, Holy, IcyBurst,
	Keen, Seeking, Shock, ShockingBurst, Speed,
	Unholy, Vicious, Vorpal, Wounding,
	Coins,
};
struct itemfi {
	const char*		id;
};
struct weaponi {
	damagen		type;
	dice		damage;
	rangen		range;
	char		bonus;
	explicit operator bool() const { return damage.c != 0; }
	void		clear();
};
struct itemi : nameable {
	unsigned	count, weight, cost;
	short		avatar, ground;
	rfpma*		equiped;
	rfpma*		thrown;
	weaponi		weapon;
	unsigned	flags;
	wearn		wear;
	char		magic;
	variants	use, wearing;
	itemi*		basic; // Basic item. Showm as unidentified. Can be identified.
	char		max_dex_bonus;
	featn		required;
	int			getcritical() const;
	int			getmultiplier() const;
	bool		is(itemf v) const { return (flags & (1 << v)) != 0; }
};
struct item {
	unsigned short type;
	unsigned char count; // Every item have count or damage.
	union {
		struct {
			unsigned char identified : 1; // Item is fully identified
			unsigned char personal : 1; // Item can't be dropped or remove from inventory
			unsigned char broken : 3; // 0 is new, 7 is almost broken.
		};
		unsigned char data;
	};
	constexpr item() : type(0), count(1), data(0) {}
	item(unsigned short type) : type(type), count(geti().count ? geti().count : 1), data(0) {}
	constexpr explicit operator bool() const { return type != 0; }
	void		add(item& v);
	bool		canequip(wearn v) const;
	void		clear() { type = 0; count = 0; data = 0; }
	void		identify(int v) { identified = v; }
	bool		is(itemf v) const { return geti().is(v); }
	bool		is(wearn v) const;
	bool		isweapon() const { return geti().weapon.operator bool(); }
	int			getcost() const { return geti().cost; }
	int			getcostall() const;
	const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	void		getinfo(stringbuilder& sb) const;
	const char*	getname() const;
	int			getweight() const { return count * geti().weight; }
	void		setcount(int v);
};
extern item* last_item;
extern item* party_item;
extern item* store_item;