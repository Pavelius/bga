#include "ability.h"
#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "draw_control.h"
#include "draw_object.h"
#include "log.h"
#include "resid.h"

using namespace draw;

void check_translation();
void initialize_translation(const char* locale);
void util_main();

static void start_main() {
	last_form = bsdata<form>::find("GUICHP1B");
	if(last_form)
		draw::scene(last_form->paintscene);
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
	colors::text = color(255, 255, 255);
	pbeforemodal = beforemodal;
	pbackground = background;
	//answers::beforepaint = answers_beforepaint;
	//answers::paintcell = menubt;
	//pfinish = finish;
	//ptips = tips;
	awindow.flags = WFResize | WFMinmax;
	metrics::border = 5;
	metrics::padding = 1;
	initialize(getnm("AppTitle"));
	setnext(start_main);
	start();
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}