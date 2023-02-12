#include "ability.h"
#include "bsreq.h"
#include "crt.h"
#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_control.h"
#include "log.h"
#include "resid.h"

using namespace draw;

void check_translation();
void initialize_translation(const char* locale);
void initialize_widgets();
void util_main();

static item& citem(const char* id) {
	static item it;
	it.clear();
	auto pi = bsdata<itemi>::find(id);
	if(pi) {
		it.type = pi - bsdata<itemi>::elements;
	}
	return it;
}

static void create_party() {
	for(auto i = 0; i < 6; i++) {
		last_creature = bsdata<creature>::add();
		last_creature->create(Male);
		last_creature->basic.abilitites[Dexterity] += 6;
		last_creature->update();
		party[i] = last_creature;
	}
}

static void start_main() {
	create_party();
	last_creature->additem(citem("BattleAxe"));
	last_creature->additem(citem("BattleAxeP1"));
	last_creature->additem(citem("LeatherArmor"));
	last_creature->additem(citem("StuddedLeatherArmor"));
	last_creature->additem(citem("ScaleMail"));
	last_creature->additem(citem("LongSword"));
	last_creature->additem(citem("HalfPlate"));
	last_creature->additem(citem("PotionOfHealing"));
	last_creature->additem(citem("Helm"));
	last_creature->additem(citem("HelmFull"));
	last_creature->additem(citem("LargeShield"));
	form::open("GUIINV08");
}

static void read_rules() {
	bsreq::read("rules/Colors.txt");
	bsreq::read("rules/Races.txt");
	bsreq::read("rules/Items.txt");
	log::readdir("forms", "*.txt", form::read);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	read_rules();
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	initialize_translation("ru");
	check_translation();
	colorgrad::initialize();
	widget::initialize();
	if(log::geterrors())
		return -1;
	metrics::font = gres(res::NORMAL);
	metrics::h1 = gres(res::STONEBIG);
	metrics::h2 = gres(res::REALMS);
	metrics::h3 = gres(res::STONEBIG);
	colors::text = color(255, 255, 255);
	initialize(getnm("AppTitle"));
	settimer(100);
	setnext(start_main);
	start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}