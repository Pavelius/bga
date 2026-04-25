#include "area.h"
#include "container.h"
#include "itemground.h"

container* last_container;

unsigned char container::index() const {
	return this - bsdata<container>::elements;
}

void container::add(item& v) {
	add_item(current_area, {index(), itemground::Container}, v);
}