#include "script.h"
#include "list.h"

bool stop_script;

template<> void fnscript<script>(int value, int bonus) {
	auto p = bsdata<script>::elements + value;
	if(p->proc)
		p->proc(bonus);
}
template<> bool fntest<script>(int value, int bonus) {
	auto p = bsdata<script>::elements + value;
	if(p->test)
		return p->test(bonus);
	return true;
}

template<> void fnscript<listi>(int value, int bonus) {
	script::run(bsdata<listi>::elements[value].elements);
}
template<> bool fntest<listi>(int value, int bonus) {
	return script::allow(bsdata<listi>::elements[value].elements);
}

void script::run(const char* id, int bonus) {
	auto p = bsdata<script>::find(id);
	if(p && p->proc)
		p->proc(bonus);
}

void script::run(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	if(ei.pscript)
		ei.pscript(v.value, v.counter);
}

void script::run(const variants& source) {
	if(stop_script)
		return;
	auto push_stop = stop_script;
	for(auto v : source) {
		if(stop_script)
			break;
		run(v);
	}
	stop_script = push_stop;
}

bool script::allow(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	return ei.ptest ? ei.ptest(v.value, v.counter) : true;
}

bool script::allow(const variants& source) {
	for(auto v : source) {
		if(!allow(v))
			return false;
	}
	return true;
}