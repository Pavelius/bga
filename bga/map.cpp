#include "direction.h"
#include "math.h"
#include "rand.h"

//namespace {
//struct nodei {
//	indext			index;
//	short unsigned	next;
//	void			clear() { index = Blocked; next = Blocked; }
//	constexpr explicit operator bool() const { return index != Blocked; }
//};
//}
//
//static directionn all_aroud[] = {Left, Right, Up, Down, LeftUp, LeftDown, RightUp, RightDown};
//static unsigned short path_stack[256 * 256];
//static unsigned short path_cost[256 * 256];
//static nodei path_node[1024 * 8];

//static nodei* addnode() {
//	for(auto& e : path_node) {
//		if(!e)
//			return &e;
//	}
//	return 0;
//}
//
//static void addnode(short unsigned& ni, indext index) {
//	auto p = addnode();
//	if(!p)
//		return;
//	p->index = index;
//	p->next = ni;
//	ni = p - path_node;
//}

//unsigned short map::getcost(indext index) {
//	return path_cost[index];
//}
//
//void map::setcost(indext index, short unsigned value) {
//	path_cost[index] = value;
//}

//short unsigned map::getnextpath(short unsigned node) {
//	if(node == Blocked)
//		return Blocked;
//	return path_node[node].next;
//}
//
//indext map::getpathindex(short unsigned node) {
//	if(node == Blocked)
//		return Blocked;
//	return path_node[node].index;
//}
//
//indext map::getpathgoal(short unsigned node) {
//	indext r = Blocked;
//	while(node != Blocked) {
//		r = path_node[node].index;
//		node = path_node[node].next;
//	}
//	return r;
//}

//void map::removeall(short unsigned& start) {
//	while(start != Blocked)
//		remove(start);
//}
//
//void map::remove(short unsigned& start) {
//	auto p = path_node + start;
//	start = p->next;
//	p->clear();
//}

//int map::getrange(indext i0, indext i1) {
//	int x0 = getx(i0); int y0 = gety(i0);
//	int x1 = getx(i1); int y1 = gety(i1);
//	return imax(iabs(x0 - x1), iabs(y0 - y1));
//}

// First, make wave and see what cell on map is passable
//void map::createwave(indext start, int size) {
//	short unsigned path_push = 0;
//	short unsigned path_pop = 0;
//	path_stack[path_push++] = start;
//	path_cost[start] = 1;
//	while(path_push != path_pop) {
//		auto n = path_stack[path_pop++];
//		auto w = path_cost[n] + 1;
//		for(auto d : all_aroud) {
//			auto i = to(n, d);
//			if(path_cost[i] == Blocked)
//				continue;
//			if(!path_cost[i] || path_cost[i] > w) {
//				path_cost[i] = w;
//				path_stack[path_push++] = i;
//			}
//		}
//	}
//}
//
//void map::createwave(indext start, int size, short unsigned max_cost) {
//	short unsigned path_push = 0;
//	short unsigned path_pop = 0;
//	path_stack[path_push++] = start;
//	path_cost[start] = 1;
//	while(path_push != path_pop) {
//		auto n = path_stack[path_pop++];
//		auto w = path_cost[n] + 1;
//		if(w > max_cost)
//			continue;
//		for(auto d : all_aroud) {
//			auto i = to(n, d);
//			if(path_cost[i] == Blocked)
//				continue;
//			if(!path_cost[i] || path_cost[i] > w) {
//				path_cost[i] = w;
//				path_stack[path_push++] = i;
//			}
//		}
//	}
//}
//
//indext map::stepto(indext index) {
//	auto current_index = Blocked;
//	auto current_value = Blocked;
//	for(auto d : all_aroud) {
//		auto i = to(index, d);
//		if(i == Blocked)
//			continue;
//		if(path_cost[i] < current_value) {
//			current_value = path_cost[i];
//			current_index = i;
//		}
//	}
//	return current_index;
//}
//
//indext map::stepfrom(indext index) {
//	auto current_index = Blocked;
//	auto current_value = 0;
//	for(auto d : all_aroud) {
//		auto i = to(index, d);
//		if(i >= Blocked - 1)
//			continue;
//		if(path_cost[i] > current_value) {
//			current_value = path_cost[i];
//			current_index = i;
//		}
//	}
//	return current_index;
//}

// Calculate path step by step to any cell on map analizing create_wave result.
// Go form goal to start and get lowest weight.
// When function return 'path_stack' has step by step path and 'path_push' is top of this path.
//void map::route(short unsigned& ni, indext goal, pget proc, short unsigned maximum_range) {
//	for(auto n = goal; n != Blocked && path_cost[n]; n = proc(n)) {
//		auto c = getcost(n);
//		if(maximum_range && c > maximum_range)
//			continue;
//		if(c <= 1)
//			break;
//		addnode(ni, n);
//	}
//}

// Calculate path step by step to any cell on map analizing create_wave result.
// Go from goal to start and get lowest weight.
// When function return 'path_stack' has step by step path and 'path_push' is top of this path.
//int map::reachable(indext start, pget proc, short unsigned minimal_reach) {
//	auto n = proc(start);
//	auto w = 0;
//	minimal_reach += 1; // Base cost is one
//	for(; n != Blocked && path_cost[n] >= 1; n = proc(n)) {
//		w += 1;
//		if(minimal_reach >= path_cost[n])
//			return w;
//	}
//	return w;
//}
//
//bool map::islineofsight(indext start, indext goal) {
//	return true;
//}

//indext map::getminimalcost(indext start, int maximum_range, bool need_line_of_sight) {
//	if(start == Blocked)
//		return Blocked;
//	if(!maximum_range) {
//		if(getcost(start) == Blocked)
//			return Blocked;
//		return start;
//	}
//	auto x1 = getx(start);
//	auto y1 = gety(start);
//	indext result = start;
//	indext result_cost = getcost(start);
//	for(auto r = maximum_range; r > 0; r--) {
//		auto x2 = x1 + r;
//		for(auto x = x1 - r; x <= x2; x++) {
//			if(x < 0 || x >= map::width)
//				continue;
//			auto y2 = y1 + r;
//			for(auto y = y1 - r; y <= y2; y++) {
//				if(y < 0 || y >= map::height)
//					continue;
//				auto i = m2i(x, y);
//				auto c = getcost(i);
//				if(c >= result_cost)
//					continue;
//				if(need_line_of_sight) {
//					if(!islineofsight(i, start))
//						continue;
//				}
//				result = i;
//				result_cost = c;
//			}
//		}
//	}
//	return result;
//}

//unsigned short map::getdistance(indext start, indext goal) {
//	if(start == Blocked || goal == Blocked)
//		return Blocked;
//	auto dx = iabs(getx(start) - getx(goal));
//	auto dy = iabs(gety(start) - gety(goal));
//	return dx > dy ? dx : dy;
//}

//void map::initialize() {
//	for(auto& e : path_node)
//		e.clear();
//}