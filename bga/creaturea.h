#pragma once

#include "creature.h"
#include "collection.h"

typedef collection<creature> creaturea;

extern creaturea creatures; // All creatures in area
extern bool need_update_creatures;

void update_creatures();
