#include "ability.h"
#include "bsreq.h"
#include "crt.h"
#include "colorgrad.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_control.h"
#include "draw_object.h"
#include "log.h"
#include "resid.h"

using namespace draw;

void check_translation();
void initialize_translation(const char* locale);
void initialize_widgets();
void initialize_ui();
void util_main();

static void start_main() {
	//clear_indecies();
	//set_color("HairNormal");
	//default_color = 10;
	form::open("GUIINV08");
}

static void beforemodal() {
}

static void background() {
}

int main(int argc, char* argv[]) {
	srand(getcputime());
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	draw::object::initialize();
	bsreq::read("rules/Basic.txt");
	log::readdir("forms", "*.txt", form::read);
	initialize_translation("ru");
	initialize_widgets();
	initialize_ui();
	check_translation();
	if(log::geterrors())
		return -1;
	metrics::font = gres(res::NORMAL);
	metrics::h1 = gres(res::STONEBIG);
	metrics::h2 = gres(res::STONEBIG);
	metrics::h3 = gres(res::STONEBIG);
	colors::text = color(255, 255, 255);
	pbeforemodal = beforemodal;
	pbackground = background;
	awindow.flags = WFResize | WFMinmax;
	metrics::border = 5;
	metrics::padding = 1;
	initialize(getnm("AppTitle"));
	settimer(100);
	setnext(start_main);
	start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}