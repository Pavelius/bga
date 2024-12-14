#include "dice.h"
#include "rand.h"

int dice::roll() const {
	if(!d)
		return c;
	int r = b;
	for(auto i = c; i > 0; i--)
		r += 1 + rand() % d;
	return r;
}