#include "area.h"
#include "creature.h"
#include "direction.h"

unsigned short area_cost[256 * 256];
static unsigned short path_stack[256 * 256];
static directionn all_aroud[] = {Left, Right, Up, Down};
static directionn all_aroud_dialgonal[] = {LeftUp, LeftDown, RightUp, RightDown};

void clear_path_map() {
	for(auto y = 0; y < area_height; y++) {
		auto i2 = m2i(area_width, y);
		for(auto i = m2i(0, y); i < i2; i++)
			area_cost[i] = is_block(i) ? Blocked : PassableMaximum;
	}
}

static void block_area(short unsigned start, int size) {
	int x1 = i2x(start), x2 = x1 + size;
	int y1 = i2y(start), y2 = y1 + size;
	for(auto y = y1; y < y2; y++) {
		if(y >= area_height)
			break;
		for(auto x = x1; x < x2; x++) {
			if(x >= area_width)
				continue;
			area_cost[m2i(x, y)] = (x == x1) ? ((y == y1) ? BlockedCreature : BlockedLeft) : BlockedUp;
		}
	}
}

void block_creatures(fnvisible allow, bool keep) {
	for(auto& e : bsdata<creature>()) {
		if(!e || !e.ispresent())
			continue;
		if(allow(&e) != keep)
			continue;
		block_area(e.position_index, e.getsize());
	}
}

void block_creatures() {
	for(auto& e : bsdata<creature>()) {
		if(!e || !e.ispresent())
			continue;
		block_area(e.position_index, e.getsize());
	}
}

void block_movement(int range) {
	for(auto y = 0; y < area_height; y++) {
		for(auto x = 0; x < area_width; x++) {
			auto i = m2i(x, y);
			if(area_cost[i] >= Blocked)
				continue;
			if(area_cost[i] <= range)
				continue;
			area_cost[i] = Blocked;
		}
	}
}

void create_wave(short unsigned start, int size) {
	short unsigned path_push = 0;
	short unsigned path_pop = 0;
	path_stack[path_push++] = start;
	area_cost[start] = 0;
	while(path_push != path_pop) {
		auto n = path_stack[path_pop++];
		auto w = area_cost[n] + 2;
		for(auto d : all_aroud) {
			auto i = to(n, d);
			if(i == Blocked)
				continue;
			if(area_cost[i] >= Blocked || area_cost[i] <= w)
				continue;
			area_cost[i] = w;
			path_stack[path_push++] = i;
		}
		w += 1;
		for(auto d : all_aroud_dialgonal) {
			auto i = to(n, d);
			if(i == Blocked)
				continue;
			if(area_cost[i] >= Blocked || area_cost[i] <= w)
				continue;
			area_cost[i] = w;
			path_stack[path_push++] = i;
		}
	}
}

void change_cost(short unsigned v1, short unsigned v2) {
	for(auto y = 0; y < area_height; y++) {
		for(auto x = 0; x < area_width; x++) {
			auto i = m2i(x, y);
			if(area_cost[i] == v1)
				area_cost[i] = v2;
		}
	}
}

short unsigned get_cost(short unsigned index) {
	while(area_cost[index] == BlockedUp)
		index -= 256;
	while(area_cost[index] == BlockedLeft)
		index -= 1;
	return area_cost[index];
}

short unsigned nearest_cost(short unsigned target) {
	short unsigned r = 0xFFFF;
	short unsigned c = 0xFFFF;
	for(auto d : all_aroud) {
		auto i = to(target, d);
		if(i == 0xFFFF)
			continue;
		auto a = area_cost[i];
		if(a >= Blocked)
			continue;
		if(a < c) {
			c = a;
			r = i;
		}
	}
	return r;
}

static short unsigned nearest_ih(short unsigned target) {
	auto r = nearest_cost(target);
	if(r == 0xFFFF)
		return r;
	for(auto n = to(target, Right); n != 0xFFFF && area_cost[n] == BlockedLeft; n = to(n, Right)) {
		auto m = nearest_cost(n);
		if(area_cost[r] > area_cost[m])
			r = m;
	}
	return r;
}

short unsigned nearest_index(short unsigned target) {
	auto r = nearest_ih(target);
	if(r == 0xFFFF)
		return r;
	for(auto n = to(target, Down); n != 0xFFFF && area_cost[n] == BlockedUp; n = to(n, Down)) {
		auto m = nearest_ih(n);
		if(area_cost[r] > area_cost[m])
			r = m;
	}
	return r;
}

short unsigned nearest_index(short unsigned target, int range) {
	auto r = 0xFFFF;
	for(auto i = 0; i < range; i++) {
		auto n = nearest_cost(r);
		if(n == 0xFFFF)
			break;
		r = n;
	}
	return r;
}