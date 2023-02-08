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
void initialize_ui();
void util_main();

static void start_main() {
	last_creature = bsdata<creature>::add();
	last_creature->create(Female);
	//clear_indecies();
	//set_color("HairNormal");
	//default_color = 10;
	form::open("GUIINV08");
}

static void beforemodal() {
}

static void background() {
}

static void read_rules() {
	bsreq::read("rules/Colors.txt");
	bsreq::read("rules/Races.txt");
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
	initialize_widgets();
	initialize_ui();
	if(log::geterrors())
		return -1;
	metrics::font = gres(res::NORMAL);
	metrics::h1 = gres(res::STONEBIG);
	metrics::h2 = gres(res::REALMS);
	metrics::h3 = gres(res::STONEBIG);
	colors::text = color(255, 255, 255);
	pbeforemodal = beforemodal;
	pbackground = background;
	initialize(getnm("AppTitle"));
	settimer(100);
	setnext(start_main);
	start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}