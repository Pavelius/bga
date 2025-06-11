#include "ability.h"
#include "area.h"
#include "bsreq.h"
#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "log.h"
#include "itema.h"
#include "rand.h"
#include "resid.h"
#include "script.h"
#include "spell.h"
#include "timer.h"
#include "view.h"
#include "worldmap.h"

using namespace draw;

void main_identifier(stringbuilder& sb, const char* identifier);
void initialize_translation();
void initialize_store();
void util_main();

static item& citem(const char* id, int count = 1) {
	static item it;
	it.clear();
	auto pi = bsdata<itemi>::find(id);
	if(pi)
		it.type = pi - bsdata<itemi>::elements;
	it.count = count;
	return it;
}

static void create_party() {
	print("Create random party...");
	for(auto i = 0; i < 6; i++) {
		create_character(Male);
		player->update();
		player->addcoins(xrand(3, 18));
		party[i] = player;
		add_player_spellbooks();
	}
}

static void add_enemies() {
	auto push_player = player;
	create_npc({744, 1049}, "GoblinWarrior");
	player->feats.set(Enemy);
	player = push_player;
}

static void start_main() {
	last_screen.set(0, 0, 800, 433);
	create_game();
	create_party();
	select_all_party();
	enter("AR1000", "FR1001");
	add_enemies();
	player->additem(citem("BattleAxe"));
	player->additem(citem("BattleAxeP1"));
	player->additem(citem("LeatherArmor"));
	player->additem(citem("StuddedLeatherArmor"));
	player->additem(citem("ScaleMail"));
	player->additem(citem("LongSwordFlaming"));
	player->additem(citem("HalfPlate"));
	player->additem(citem("PotionOfHealing", 10));
	player->additem(citem("Helm"));
	player->additem(citem("HelmFull"));
	player->additem(citem("LargeShield"));
	player->additem(citem("TwoHandedSword"));
	player->update();
	current_world = bsdata<worldmapi>::elements;
	next_scene(check_combat);
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
	bsreq::read("rules/Spells.txt");
	bsreq::read("rules/Diety.txt");
	bsreq::read("rules/Monsters.txt");
}

int main(int argc, char* argv[]) {
	// srand(getcputime());
	srand(923811);
	stringbuilder::custom = main_identifier;
	read_rules();
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	initialize_translation();
	initialize_store();
	initialize_colorgrad();
	initialize_ui();
	if(log::errors)
		return -1;
	metrics::font = gres(NORMAL);
	metrics::h1 = gres(STONEBIG);
	metrics::h2 = gres(REALMS);
	metrics::h3 = gres(NORMAL);
	colors::text = color(255, 255, 255);
	colors::h3 = color(244, 214, 66);
	colors::h1 = colors::text.mix(colors::h3, 64);
	colors::h2 = colors::text.mix(colors::h3, 96);
	colors::special = color(244, 214, 66);
	initialize(getnm("AppTitle"));
	settimer(64);
	next_scene(start_main);
	start_scene();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}