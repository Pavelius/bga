#include "crt.h"
#include "draw_gui.h"

using namespace draw;

guii draw::gui;

void guii::clear() {
	memset(this, 0, sizeof(*this));
}