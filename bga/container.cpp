#include "container.h"

container* last_container;

unsigned char container::index() const {
	return this - bsdata<container>::elements;
}