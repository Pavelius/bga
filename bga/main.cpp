#include "ability.h"
#include "area.h"
#include "bsreq.h"
#include "crt.h"
#include "colorgrad.h"
#include "creature.h"
#include "door.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_control.h"
#include "log.h"
#include "resid.h"

using namespace draw;

void add_console(const char* format);
void check_translation();
void initialize_translation(const char* locale);
void read_descriptions(const char* folder);
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
	for(auto i = 0; i < 6; i++) {
		player = bsdata<creature>::add();
		player->create(Male);
		player->update();
		party[i] = player;
	}
}

static void start_main() {
	add_console("Read area...");
	map::read("AR1000");
	camera = {300, 1000};
	add_console("Create party...");
	create_party();
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
	form::nextscene("GUIINV08");
}

static void read_rules() {
	bsreq::read("rules/Colors.txt");
	bsreq::read("rules/Races.txt");
	bsreq::read("rules/Classes.txt");
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Advance.txt");
	log::readdir("forms", "*.txt", form::read);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	read_rules();
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	initialize_translation("ru");
	read_descriptions("area");
	check_translation();
	colorgrad::initialize();
	widget::initialize();
	if(log::geterrors())
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
	settimer(100);
	setnext(start_main);
	start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}