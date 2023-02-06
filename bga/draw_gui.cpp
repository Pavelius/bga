#include "crt.h"
#include "draw_gui.h"

using namespace draw;

guii draw::gui;
fnevent draw::guii::apply;

void guii::clear() {
	memset(this, 0, sizeof(*this));
}