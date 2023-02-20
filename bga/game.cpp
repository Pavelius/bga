#include "area.h"
#include "console.h"
#include "entrance.h"
#include "draw_control.h"
#include "game.h"

void enter(const char* id, const char* location) {
	char temp[32]; stringbuilder sb(temp); sb.add(location);
	logm("Enter area [%1] at location [%2]", id, location);
	map::read(id);
	auto pn = entrance::find(temp);
	if(pn)
		setcamera(pn->position);
	draw::form::nextscene("GGAME");
}
