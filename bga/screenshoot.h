#include "point.h"
#include "draw.h"

#pragma once

struct screenshoot : public point, public draw::surface {
	screenshoot(bool fade = false);
	screenshoot(rect rc, bool fade = false);
	~screenshoot();
	void restore();
};
long open_dialog(fnevent proc, bool faded);