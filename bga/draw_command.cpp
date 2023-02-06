#include "crt.h"
#include "draw_command.h"

void draw::command::execute(const char* id) {
	if(!id || id[0] == 0)
		return;
	for(auto& e : bsdata<command>()) {
		if(equal(e.id, id)) {
			e.proc();
			break;
		}
	}
}