#include "ability.h"
#include "bsreq.h"
#include "color.h"
#include "colorgrad.h"
#include "crt.h"
#include "draw_control.h"
#include "item.h"
#include "race.h"
#include "variant.h"
#include "widget.h"

using namespace draw;

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(itemi, 512)
BSDATAD(variant)
BSDATAC(colorgrad, 32)
BSDATAC(control, 1024)
BSDATAC(form, 128)

NOBSDATA(color)
NOBSDATA(dice)
NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(color) = {
	BSREQ(r), BSREQ(g), BSREQ(b),
	{}};
BSMETA(colorgrad) = {
	BSREQ(id), BSREQ(indecies),
	{}};
BSMETA(control) = {
	BSREQ(visual), BSREQ(resource), BSREQ(x), BSREQ(y), BSREQ(width), BSREQ(height),
	BSREQ(value), BSREQ(frames), BSREQ(fore), BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(form) = {
	BSREQ(id),
	BSREQ(controls),
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
BSMETA(racei) = {
	BSREQ(id),
	BSREQ(skin), BSREQ(hairs),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(residi) = {
	BSREQ(id),
	{}};
BSMETA(weaponi) = {
	BSREQ(bonus), BSREQ(ac), BSREQ(magic),
	BSREQ(damage),
	BSENM(range, rangei),
	{}};
BSMETA(widget) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Control", VAR(control, 6)},
	{"ColorGrad", VAR(colorgrad, 1)},
	{"Item", VAR(itemi, 1)},
	{"Rate", VAR(magici, 1)},
	{"Resource", VAR(residi, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)
