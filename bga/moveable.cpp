#include "moveable.h"

void moveable::movestep(int speed) {
	if(!ismoving())
		return;
	auto n = distance(move_start, position);
	auto m = distance(move_start, move_stop);
	n += speed;
	if(n >= m) {
		position = move_stop;
		move_stop.clear();
		move_start.clear();
	} else {
		position.x = move_start.x + n * (move_stop.x - move_start.x) / m;
		position.y = move_start.y + n * (move_stop.y - move_start.y) / m;
	}
}