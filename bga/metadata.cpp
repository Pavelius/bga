#include "ability.h"
#include "advance.h"
#include "bsreq.h"
#include "color.h"
#include "crt.h"
#include "creature.h"
#include "door.h"
#include "draw_command.h"
#include "draw_control.h"
#include "gender.h"
#include "item.h"
#include "modifier.h"
#include "portrait.h"
#include "race.h"
#include "region.h"
#include "script.h"
#include "skill.h"
#include "variant.h"
#include "widget.h"

using namespace draw;

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(advancei, 512)
BSDATAC(colorgrad, 32)
BSDATAC(control, 1024)
BSDATAC(creature, 256)
BSDATAC(door, 64)
BSDATAC(doortile, 256)
BSDATAC(itemi, 512)
BSDATAC(form, 128)
BSDATAC(point, 256*256)
BSDATAC(racei, 32)
BSDATAC(region, 128)
BSDATAD(variant)

NOBSDATA(color)
NOBSDATA(dice)
NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(parent), BSREQ(level), BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(classi) = {
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
	BSREQ(value), BSREQ(frames), BSREQ(fore),
	BSREQ(id), BSREQ(data),
	{}};
BSMETA(command) = {
	BSREQ(id),
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(form) = {
	BSREQ(id),
	BSREQ(controls),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(basic),
	BSREQ(weapon),
	BSREQ(required),
	BSREQ(count), BSREQ(weight), BSREQ(cost),
	BSFLG(flags, itemfi),
	BSREQ(magic), BSREQ(max_dex_bonus),
	BSREQ(avatar), BSREQ(ground), BSENM(equiped, residi), BSENM(thrown, residi),
	BSENM(wear, weari),
	BSREQ(wearing), BSREQ(use),
	{}};
BSMETA(itemfi) = {
	BSREQ(id),
	{}};
BSMETA(magici) = {
	BSREQ(id),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(portraiti) = {
	BSREQ(id),
	BSREQ(colors),
	BSENM(gender, genderi),
	BSENM(race, racei),
	BSENM(classv, classi),
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
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	BSFLG(classes, classi),
	{}};
BSMETA(weaponi) = {
	BSREQ(bonus),
	BSENM(type, damagei),
	BSREQ(damage),
	BSENM(range, rangei),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSMETA(widget) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Advance", VAR(advancei, 3)},
	{"Class", VAR(classi, 1)},
	{"Control", VAR(control, 6)},
	{"Command", VAR(command, 1)},
	{"ColorGrad", VAR(colorgrad, 1)},
	{"Feat", VAR(feati, 1), 0, 0, fnscript<feati>},
	{"Form", VAR(form, 1)},
	{"Item", VAR(itemi, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Portrait", VAR(portraiti, 1)},
	{"Race", VAR(racei, 1)},
	{"Rate", VAR(magici, 1)},
	{"Resource", VAR(residi, 1)},
	{"Skill", VAR(skilli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)