#include "log.h"
#include "logvalue.h"
#include "pushvalue.h"
#include "variant.h"

using namespace log;

static char temp[512];

//static const char* skip(const char* p, char sym) {
//	if(*p == sym)
//		return skipws(p + 1);
//	char temp[2] = {sym, 0};
//	log::errorp(p, "Expected symbol `%1`", temp);
//	return skipws(p);
//}
//
//static void save_value(void* object, long number, int size) {
//	switch(size) {
//	case 1: *((unsigned char*)object) = (unsigned char)number; break;
//	case 2: *((unsigned short*)object) = (unsigned short)number; break;
//	case 4: *((unsigned*)object) = (unsigned)number; break;
//	default: break;
//	}
//}

//static const char* read_value(const char* p, void* object, const bsreq* req, int index) {
//	if(*p == '(') {
//		p = skipws(p + 1);
//		auto index = 0;
//		if(req->count > 1) {
//			while(*p)
//				p = read_value(p, object, req, index++);
//		} else if(req[1]) {
//			while(*p)
//				p = read_value(p, object, req + (index++), 0);
//		} else
//			p = read_value(p, object, req, 0);
//		return skip(p, ')');
//	} else {
//		stringbuilder sb(temp);
//		value.clear();
//		p = skipws(value.read(p, sb));
//		if(req->is(KindReference))
//			save_value(req->ptr(object, index), (long)value.data, req->size);
//		else if(req->is(KindNumber))
//			save_value(req->ptr(object, index), value.number, req->size);
//	}
//	return p;
//}

//static const char* read_block(const char* p) {
//	stringbuilder sb(temp);
//	p = skipws(psidf(p, sb));
//	auto pi = bsdata<varianti>::find(sb.begin());
//	if(!pi) {
//		log::errorp(p, "Not found variant type `%1`", sb.begin());
//		return skipline(p);
//	}
//	if(!pi->source) {
//		log::errorp(p, "Can't find source array in data `%1`", pi->id);
//		return skipline(p);
//	}
//	// Find array and create or choose objects.
//	void* object = 0;
//	if(pi->key_count) {
//		object = pi->source->add();
//		for(auto i = 1; i < pi->key_count; i++)
//			p = read_value(p, object, pi->metadata + i, 0);
//	} else {
//		int index = 0;
//		p = psnum(p, index);
//		object = pi->source->ptr(index);
//	}
//	return p;
//}

static const char* read_block(const char* p) {
	stringbuilder sb(temp);
	p = skipws(psidf(p, sb));
	auto pi = bsdata<varianti>::find(sb.begin());
	if(!pi) {
		log::errorp(p, "Not found variant type `%1`", sb.begin());
		return skipline(p);
	}
	if(!pi->source) {
		log::errorp(p, "Can't find source array in data `%1`", pi->id);
		return skipline(p);
	}
	return p;
}

void area_read(const char* url) {
	pushvalue push(log::context);
	auto p = log::read(url);
	if(!p)
		return;
	p = skipwscr(p);
	while(*p) {
		p = read_block(p);
		p = skipwscr(p);
	}
	log::close();
}