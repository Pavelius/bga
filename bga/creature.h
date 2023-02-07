#include "coloration.h"
#include "portrait.h"
#include "wearable.h"

#pragma once

struct creature : wearable, avatarable, coloration {
};
extern creature* last_creature;
