#include "point.h"
#include "draw.h"

#pragma once

namespace draw {
struct screenshoot : public point, public surface {
	screenshoot(bool fade = false);
	screenshoot(rect rc, bool fade = false);
	~screenshoot();
	static long			open(fnevent proc, bool faded);
	void				restore();
};
}