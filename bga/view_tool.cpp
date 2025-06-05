#include "draw.h"
#include "resid.h"
#include "view.h"

using namespace draw;

#ifdef _DEBUG

void util_items_list();

void input_debug() {
	switch(hot.key) {
	case Ctrl + 'I': execute(open_scene, 0, 0, util_items_list); break;
	default: break;
	}
}

#else

void input_debug() {
}

#endif // DEBUG