#include "draw.h"
#include "draw_command.h"
#include "script.h"

static void test(int bonus) {

}

BSDATA(script) = {
	{"Test", test}
};
BSDATAF(script)

BSDATA(draw::command) = {
	{"Cancel", draw::buttoncancel, KeyEscape},
	{"Done", draw::buttonok, KeyEnter},
};
BSDATAF(draw::command)