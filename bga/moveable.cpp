#include "bsdata.h"
#include "creature.h"
#include "container.h"
#include "door.h"
#include "region.h"
#include "moveable.h"

//template<> orderable::operator creature*() const {
//	if(type == Creature)
//		return bsdata<creature>::elements + target;
//	return 0;
//}
//
//template<> orderable::operator door*() const {
//	if(type == Door)
//		return bsdata<door>::elements + target;
//	return 0;
//}
//
//template<> orderable::operator region*() const {
//	if(type == Region)
//		return bsdata<region>::elements + target;
//	return 0;
//}
//
//template<> orderable::operator container*() const {
//	if(type == Container)
//		return bsdata<container>::elements + target;
//	return 0;
//}
//
//template<> void orderable::operator=(const creature* v) {
//	if(v) {
//		type = Creature;
//		target = v - bsdata<creature>::elements;
//	} else {
//		type = NoType;
//		target = 0;
//	}
//}
//
//template<> void orderable::operator=(const container* v) {
//	if(v) {
//		type = Container;
//		target = v - bsdata<container>::elements;
//	} else {
//		type = NoType;
//		target = 0;
//	}
//}
//
//template<> void orderable::operator=(const door* v) {
//	if(v) {
//		type = Door;
//		target = v - bsdata<door>::elements;
//	} else {
//		type = NoType;
//		target = 0;
//	}
//}
//
//template<> void orderable::operator=(const region* v) {
//	if(v) {
//		type = Region;
//		target = v - bsdata<region>::elements;
//	} else {
//		type = NoType;
//		target = 0;
//	}
//}

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