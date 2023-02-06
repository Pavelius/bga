#include "ability.h"
#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_control.h"
#include "draw_object.h"
#include "log.h"
#include "resid.h"

using namespace draw;

void check_translation();
void initialize_translation(const char* locale);
void util_main();

static void chapter_prepare() {
	if(equal(gui.id, "Description")) {
		gui.text = "Uncle Tad always sad `You must follow the rabbit`. And I always follow, when rabit is in zone of my sight.";
	}
}

static void start_main() {
	auto push_prepare = form::prepare;
	last_form = bsdata<form>::find("GUICHP0B");
	form::prepare = chapter_prepare;
	if(last_form)
		draw::scene(last_form->paintscene);
	form::prepare = push_prepare;
}

static void initialize() {
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
	initialize();
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