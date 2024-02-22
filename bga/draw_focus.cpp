#include "draw_focus.h"

void* draw::current_focus;

void draw::setfocus(void* value) {
	current_focus = value;
}