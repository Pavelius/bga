#include "formation.h"
#include "map.h"
#include "math.h"

#define M_PI_2 1.57079632679489661923 // pi/2
#define M_PI 3.14159265358979323846 // pi

extern "C" double atan(double x);
extern "C" double cos(double x);
extern "C" double sin(double x);

formation_s current_formation = FormationProtect;

static point formations[10][20] = {
	{{0, 0}, {0, 36}, {0, 72}, {0, 108}, {0, 144}, {0, 180}, {0, 216}, {0, 252}, {0, 288}, {0, 324}, {0, 360}, {0, 396}, {0, 432}, {0, 468}, {0, 504}, {0, 540}, {0, 576}, {0, 612}, {0, 648}, {0, 684}}, // Follow
	{{0, 0}, {48, 0}, {-48, 0}, {0, 48}, {0, 84}, {0, 120}, {0, 156}, {0, 192}, {0, 228}, {0, 264}, {0, 300}, {0, 336}, {0, 372}, {0, 408}, {0, 444}, {0, 480}, {0, 516}, {0, 552}, {0, 588}, {0, 624}}, // T
	{{0, -36}, {48, -24}, {-48, -24}, {48, 24}, {-48, 24}, {0, 36}, {48, 48}, {-48, 48}, {0, 72}, {48, 72}, {-48, 72}, {0, 108}, {48, 96}, {-48, 96}, {0, 144}, {48, 120}, {-48, 120}, {0, 180}, {48, 144}, {-48, 144}}, // Gather
	{{0, 0}, {64, 0}, {-64, 0}, {128, 0}, {0, 48}, {64, 48}, {-64, 48}, {128, 48}, {0, 96}, {64, 96}, {-64, 96}, {128, 96}, {0, 144}, {64, 144}, {-64, 144}, {128, 144}, {0, 192}, {64, 192}, {-64, 192}, {128, 192}}, // 4 and 2
	{{0, 0}, {64, 0}, {-64, 0}, {0, 48}, {64, 48}, {-64, 48}, {0, 96}, {64, 96}, {-64, 96}, {0, 144}, {64, 144}, {-64, 144}, {0, 192}, {64, 192}, {-64, 192}, {0, 240}, {64, 240}, {-64, 240}, {0, 288}, {64, 288}}, // 3 by 2
	{{0, 0}, {0, -36}, {-64, 0}, {64, 0}, {-32, 48}, {32, 48}, {0, 24}, {0, 48}, {0, 72}, {0, 96}, {0, 120}, {0, 144}, {0, 168}, {0, 192}, {0, 216}, {0, 240}, {0, 264}, {0, 288}, {0, 312}, {0, 336}}, // Protect
};

point getformation(point dst, point src, formation_s formation, int pos) {
	if(formation >= sizeof(formations) / sizeof(formations[0]))
		return src;
	pos = imin(pos, (int)(sizeof(formations[0]) / sizeof(formations[0][0])));
	// calculate angle
	double angle;
	double xdiff = (double)src.x - (double)dst.x;
	double ydiff = (double)src.y - (double)dst.y;
	if(ydiff == 0) {
		if(xdiff > 0)
			angle = M_PI_2;
		else
			angle = -M_PI_2;
	} else {
		angle = atan(xdiff / ydiff);
		if(ydiff < 0)
			angle += M_PI;
	}
	// calculate new coordinates by rotating formation around (0,0)
	double newx = -formations[formation][pos].x * cos(angle) + formations[formation][pos].y * sin(angle);
	double newy = formations[formation][pos].x * sin(angle) + formations[formation][pos].y * cos(angle);
	dst.x = imin(imax(dst.x + (short)newx, 8), (short)map::width * 16);
	dst.y = imin(imax(dst.y + (short)newy, 8), (short)map::height * 12);
	return dst;
}