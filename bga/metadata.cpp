#include "ability.h"
#include "advance.h"
#include "animation.h"
#include "area.h"
#include "calendar.h"
#include "bsreq.h"
#include "color.h"
#include "crt.h"
#include "container.h"
#include "creature.h"
#include "door.h"
#include "draw_form.h"
#include "entrance.h"
#include "floattext.h"
#include "ftgetinfo.h"
#include "gender.h"
#include "geography.h"
#include "itemground.h"
#include "list.h"
#include "modifier.h"
#include "portrait.h"
#include "race.h"
#include "region.h"
#include "script.h"
#include "skill.h"
#include "store.h"
#include "variant.h"
#include "widget.h"
#include "worldmap.h"

using namespace draw;

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(advancei, 512)
BSDATAC(animation, 128)
BSDATAC(areai, 512)
BSDATAC(calendari, 128)
BSDATAC(colorgrad, 32)
BSDATAC(control, 1024)
BSDATAC(container, 128)
BSDATAC(creature, 256)
BSDATAC(door, 64)
BSDATAC(doortile, 256)
BSDATAC(entrance, 64)
BSDATAC(floattext, 64)
BSDATAC(itemi, 512)
BSDATAC(itemground, 512)
BSDATAC(form, 128)
BSDATAC(point, 256*256)
BSDATAC(region, 128)
BSDATAD(variant)
BSDATAC(variable, 1024)
BSDATAC(worldmapi, 8)
BSDATAC(worldmapi::area, 128)
BSDATAC(worldmapi::link, 512)

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
BSMETA(calendari) = {
	BSREQ(id),
	BSENM(period, periodi),
	BSREQ(duration),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(hit_points), BSREQ(skill_points),
	BSFLG(skills, skilli),
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
	BSREQ(hotkey),
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	BSREQ(require),
	{}};
BSMETA(form) = {
	BSREQ(id),
	BSREQ(controls),
	{}};
BSMETA(geographyi) = {
	BSREQ(id),
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
BSMETA(periodi) = {
	BSREQ(id),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
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
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	{}};
BSMETA(storei) = {
	BSREQ(id),
	BSENM(type, storeti),
	BSFLG(flags, storefi),
	BSREQ(buy_percent), BSREQ(sell_percent), BSREQ(chance_steal),
	BSREQ(room_prices),
	{}};
BSMETA(storefi) = {
	BSREQ(id),
	{}};
BSMETA(storeti) = {
	BSREQ(id),
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
BSMETA(worldmapi) = {
	BSREQ(id),
	BSREQ(background),
	BSREQ(icons),
	{}};
BSMETA(worldmapi::area) = {
	BSREQ(id),
	BSREQ(position),
	BSREQ(avatar),
	BSREQ(realm),
	{}};
BSMETA(worldmapi::link) = {
	BSREQ(from), BSREQ(to),
	BSREQ(entry),
	BSENM(side, geographyi),
	BSREQ(time), BSREQ(encounter_chance),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Advance", VAR(advancei, 3)},
	{"Calendar", VAR(calendari, 1)},
	{"Class", VAR(classi, 1)},
	{"Control", VAR(control, 6)},
	{"ColorGrad", VAR(colorgrad, 1)},
	{"DayPart", VAR(periodi, 1)},
	{"Feat", VAR(feati, 1), 0, 0, fnscript<feati>},
	{"Form", VAR(form, 1)},
	{"Item", VAR(itemi, 1)},
	{"List", VAR(listi, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Portrait", VAR(portraiti, 1)},
	{"Race", VAR(racei, 1)},
	{"Rate", VAR(magici, 1)},
	{"Resource", VAR(residi, 1)},
	{"Script", VAR(script, 1)},
	{"Skill", VAR(skilli, 1), ftgetinfo<skilli>},
	{"Store", VAR(storei, 1)},
	{"Type", VAR(varianti, 1)},
	{"Widget", VAR(widget, 1)},
	{"WorldArea", VAR(worldmapi::area, 1)},
	{"WorldLink", VAR(worldmapi::link, 3)},
	{"WorldRealm", VAR(worldmapi, 1)},
};
BSDATAF(varianti)