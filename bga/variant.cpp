#include "variant.h"

static const char* match(const char* text, const char* name) {
	while(*name) {
		if(*name++ != *text++)
			return 0;
	}
	return text;
}

template<> variant::variant(const void* v) : u(0) {
	for(auto& e : bsdata<varianti>()) {
		if(!e.source)
			continue;
		auto i = e.source->indexof(v);
		if(i != -1) {
			value = i;
			type = &e - bsdata<varianti>::elements;
			break;
		}
	}
}

template<> variant::variant(const char* v) : u(0) {
	if(v) {
		auto size = zlen(v) + 1;
		for(auto& e : bsdata<varianti>()) {
			if(!e.source || !e.metadata || e.key_count != 1)
				continue;
			int i = e.source->findps(v, 0, size);
			if(i != -1) {
				value = i;
				type = &e - bsdata<varianti>::elements;
				counter = 0;
				break;
			}
		}
	}
}

const char* variant::getname() const {
	auto& e = geti();
	if(!e.source)
		return getnm("NoVariant");
	auto object = e.source->ptr(value);
	if(e.key_count == 1) {
		auto id = *((const char**)object);
		if(id)
			return getnm(id);
	}
	return getnm("NoName");
}

const char* variant::getid() const {
	auto& e = geti();
	if(!e.source)
		return "NoVariant";
	auto object = e.source->ptr(value);
	if(e.key_count == 1) {
		auto id = *((const char**)object);
		if(id)
			return id;
	}
	return "NoName";
}

void variant::getinfo(stringbuilder& sb) const {
	auto& e = geti();
	if(!e.source)
		return;
	if(e.pgetinfo)
		e.pgetinfo(e.source->ptr(value), sb);
	else {
		auto id = getid();
		auto description = getdescription(id);
		if(description) {
			sb.add("##%1", getname());
			sb.addn(description);
		}
	}
}

const varianti* varianti::find(const void* object) {
	if(object) {
		for(auto& e : bsdata<varianti>()) {
			if(e.source && e.source->have(object))
				return &e;
		}
	}
	return 0;
}