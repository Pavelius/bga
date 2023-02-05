#include "ability.h"
#include "bsreq.h"
#include "crt.h"
#include "item.h"
#include "variant.h"
#include "widget.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(itemi, 512)
BSDATAD(variant)

NOBSDATA(dice)
NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(id_unidentified),
	BSENM(rate, magici),
	BSREQ(weapon),
	BSREQ(count), BSREQ(weight), BSREQ(cost),
	BSREQ(dress), BSREQ(use),
	{}};
BSMETA(magici) = {
	BSREQ(id),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(weaponi) = {
	BSREQ(bonus), BSREQ(ac), BSREQ(magic),
	BSREQ(damage),
	BSENM(range, rangei),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0},
	{"Item", VAR(itemi, 1), 0, 0},
	{"Rate", VAR(magici, 1), 0, 0},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)
