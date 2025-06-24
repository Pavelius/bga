#include "ability.h"
#include "advance.h"
#include "alignment.h"
#include "ambient.h"
#include "animation.h"
#include "area.h"
#include "calendar.h"
#include "bsreq.h"
#include "color.h"
#include "condition.h"
#include "container.h"
#include "creature.h"
#include "diety.h"
#include "door.h"
#include "draw.h"
#include "entrance.h"
#include "floattext.h"
#include "gender.h"
#include "geography.h"
#include "itemground.h"
#include "list.h"
#include "modifier.h"
#include "npc.h"
#include "party.h"
#include "playlist.h"
#include "portrait.h"
#include "race.h"
#include "region.h"
#include "school.h"
#include "script.h"
#include "skill.h"
#include "spell.h"
#include "store.h"
#include "stringvar.h"
#include "variant.h"
#include "worldmap.h"

using namespace draw;

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(advancei, 512)
BSDATAC(ambient, 64)
BSDATAC(animation, 128)
BSDATAC(areai, 512)
BSDATAC(calendari, 128)
BSDATAC(colorgrad, 32)
BSDATAC(container, 128)
BSDATAC(creature, 256)
BSDATAC(door, 64)
BSDATAC(dietyi, 32)
BSDATAC(doortile, 256)
BSDATAC(entrance, 64)
BSDATAC(floattext, 64)
BSDATAC(itemi, 512)
BSDATAC(itemground, 512)
BSDATAC(npci, 512)
BSDATAC(point, 256*256)
BSDATAC(partyi, 64)
BSDATAC(playlisti, 64)
BSDATAC(region, 128)
BSDATAC(spelli, 1024)
BSDATAC(spellbook, 32)
BSDATAD(variant)
BSDATAC(variable, 1024)
BSDATAC(worldmapi, 8)
BSDATAC(worldmapi::area, 128)
BSDATAC(worldmapi::link, 512)

NOBSDATA(color)
NOBSDATA(durationi)
NOBSDATA(dice)
NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(parent), BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(calendari) = {
	BSREQ(id),
	BSENM(period, periodi),
	BSREQ(duration),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(hit_points), BSREQ(skill_points),
	BSENM(cast, abilityi),
	BSFLG(skills, skilli),
	BSFLG(alignment, alignmenti),
	{}};
BSMETA(color) = {
	BSREQ(r), BSREQ(g), BSREQ(b),
	{}};
BSMETA(colorgrad) = {
	BSREQ(id), BSREQ(indecies),
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(durationi) = {
	BSREQ(base),
	BSREQ(level),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(dietyi) = {
	BSREQ(id),
	BSREQ(powers),
	BSREQ(spells),
	BSFLG(skills, skilli),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	BSREQ(require),
	{}};
BSMETA(geographyi) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(basic),
	BSREQ(weapon),
	BSENM(required, feati),
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
BSMETA(npci) = {
	BSREQ(id),
	BSREQ(res),
	BSDST(abilities, abilityi),
	BSDST(classes, classi),
	{}};
BSMETA(periodi) = {
	BSREQ(id),
	{}};
BSMETA(playlisti) = {
	BSREQ(id),
	BSREQ(folder),
	BSREQ(music),
	{}};
BSMETA(playlistni) = {
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
	BSENM(favor, classi),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(residi) = {
	BSREQ(id),
	{}};
BSMETA(schooli) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(avatar),
	BSREQ(save_difficult),
	BSREQ(duration),
	BSENM(range, rangei),
	BSENM(save, abilityi),
	BSENM(school, schooli),
	BSDST(classes, classi),
	BSREQ(wearing),
	BSREQ(instant),
	{}};
BSMETA(storei) = {
	BSREQ(id),
	BSENM(type, storeti),
	BSFLG(flags, storefi),
	BSREQ(buy_percent), BSREQ(sell_percent), BSREQ(steal_difficult),
	BSREQ(room_prices),
	BSREQ(items),
	{}};
BSMETA(storefi) = {
	BSREQ(id),
	{}};
BSMETA(storeti) = {
	BSREQ(id),
	{}};
BSMETA(stringvari) = {
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
	{"Ability", VAR(abilityi, 1), 0, 0, ftscript<abilityi>},
	{"Advance", VAR(advancei, 2)},
	{"Alignment", VAR(alignmenti, 1)},
	{"Calendar", VAR(calendari, 1)},
	{"Class", VAR(classi, 1)},
	{"ColorGrad", VAR(colorgrad, 1)},
	{"Condition", VAR(conditioni, 1), 0, 0, ftscript<conditioni>, fttest<conditioni>},
	{"DayPart", VAR(periodi, 1)},
	{"Diety", VAR(dietyi, 1)},
	{"Feat", VAR(feati, 1), 0, 0, ftscript<feati>},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1)},
	{"List", VAR(listi, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, ftscript<modifieri>},
	{"NPC", VAR(npci, 1)},
	{"Playlist", VAR(playlisti, 2)},
	{"Portrait", VAR(portraiti, 1)},
	{"Race", VAR(racei, 1)},
	{"Rate", VAR(magici, 1)},
	{"Resource", VAR(residi, 1)},
	{"Script", VAR(script, 1)},
	{"Skill", VAR(skilli, 1), ftinfo<skilli>},
	{"Spell", VAR(spelli, 1)},
	{"Store", VAR(storei, 1)},
	{"StringVar", VAR(stringvari, 1)},
	{"Type", VAR(varianti, 1)},
	{"WorldArea", VAR(worldmapi::area, 1)},
	{"WorldLink", VAR(worldmapi::link, 3)},
	{"WorldRealm", VAR(worldmapi, 1)},
};
BSDATAF(varianti)