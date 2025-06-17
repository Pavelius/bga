#pragma once

#include "nameable.h"
#include "flagable.h"

enum tagn : unsigned char {
	Opened, Locked, Trapped,
};
struct tagi : nameable {
};
typedef flagable<1, unsigned> tagf;