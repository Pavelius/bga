#include "ability.h"
#include "area.h"
#include "bsreq.h"
#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "door.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_form.h"
#include "game.h"
#include "log.h"
#include "itema.h"
#include "rand.h"
#include "resid.h"
#include "script.h"
#include "timer.h"
#include "worldmap.h"

using namespace draw;

void act_identifier(stringbuilder& sb, const char* identifier);
void initialize_translation();
void initialize_store();
void util_main();

static item& citem(const char* id) {
	static item it;
	it.clear();
	auto pi = bsdata<itemi>::find(id);
	if(pi)
		it.type = pi - bsdata<itemi>::elements;
	it.count = 1;
	return it;
}

static void create_party() {
	logm("Create party...");
	for(auto i = 0; i < 6; i++) {
		player->create(Male);
		player->update();
		player->addcoins(xrand(3, 18));
		party[i] = player;
	}
	script::run("SelectAll", 0);
}

static void start_main() {
	create_party();
	enter("AR1000", "FR1001");
	player->additem(citem("BattleAxe"));
	player->additem(citem("BattleAxeP1"));
	player->additem(citem("LeatherArmor"));
	player->additem(citem("StuddedLeatherArmor"));
	player->additem(citem("ScaleMail"));
	player->additem(citem("LongSwordFlaming"));
	player->additem(citem("HalfPlate"));
	player->additem(citem("PotionOfHealing"));
	player->additem(citem("Helm"));
	player->additem(citem("HelmFull"));
	player->additem(citem("LargeShield"));
	player->additem(citem("TwoHandedSword"));
	player->update();
	current_world = bsdata<worldmapi>::elements;
}

static void read_rules() {
	bsreq::read("rules/Colors.txt");
	bsreq::read("rules/Races.txt");
	bsreq::read("rules/Classes.txt");
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Advance.txt");
	bsreq::read("rules/Basic.txt");
	bsreq::read("rules/Store.txt");
	bsreq::read("rules/Worldmap.txt");
	bsreq::read("rules/Calendar.txt");
	bsreq::read("rules/Feats.txt");
	log::readf(form::readhead, "forms", "*.txt");
	log::readf(form::read, "forms", "*.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	stringbuilder::custom = act_identifier;
	read_rules();
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	initialize_translation();
	initialize_store();
	colorgrad::initialize();
	widget::initialize();
	if(log::errors)
		return -1;
	metrics::font = gres(res::NORMAL);
	metrics::h1 = gres(res::STONEBIG);
	metrics::h2 = gres(res::REALMS);
	metrics::h3 = gres(res::NORMAL);
	colors::text = color(255, 255, 255);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = color(244, 214, 66);
	colors::special = color(244, 214, 66);
	initialize(getnm("AppTitle"));
	settimer(60);
	setnext(start_main);
	start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}