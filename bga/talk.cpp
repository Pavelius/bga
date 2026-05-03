#include "talk.h"
#include "io_stream.h"
#include "log.h"
#include "logvalue.h"
#include "stringbuilder.h"
#include "script.h"

BSDATAD(talkei)
BSDATAD(talki)

static bool isevent(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static bool isanswer(const char* p) {
	return p[0] == '-' && isnum(p[1]);
}

static const char* read_string(const char* p, const char*& result, stringbuilder& sb) {
	sb.clear();
	if(isevent(p))
		return p;
	while(*p) {
		if(*p == '\n' || *p == '\r') {
			p = skipspcr(p);
			if(p[0] == '#' || isanswer(p))
				break;
			sb.addch('\n');
		} else
			sb.addch(*p++);
	}
	result = szdup(sb);
	return p;
}

static const char* read_text(const char* p, const char*& result, stringbuilder& sb) {
	sb.clear();
	p = sb.psstrlf(skipsp(p));
	p = skipspcr(p);
	result = szdup(sb);
	return p;
}

static void add(variants& e, variant v) {
	if(!e.count)
		e.start = bsdata<variant>::source.getcount();
	auto p = (variant*)bsdata<variant>::source.add();
	*p = v;
	e.count++;
}

static const char* read_variants(const char* p, variants& source, stringbuilder& sb) {
	while(ischa(*p)) {
		sb.clear();
		p = sb.psidf(p);
		p = skipsp(p);
		int bonus; p = psbon(p, bonus);
		p = skipsp(p);
		variant v = (const char*)sb.begin();
		if(!v)
			log::errorp(p, "Can't find variant `%1`", sb.begin());
		else
			v.counter = bonus;
		add(source, v);
	}
	return p;
}

static const char* read_answer(const char* p, stringbuilder& sb, const talkei* parent) {
	int index = 0;
	p = psnum(skipsp(p), index);
	auto pe = bsdata<talkei>::add(); pe->clear();
	pe->index = index;
	pe->parent = parent->index;
	p = read_variants(skipsp(p), pe->elements, sb);
	p = skipline(p, ':');
	p = read_text(skipsp(p), pe->text, sb);
	return p;
}

static const char* read_event(const char* p, stringbuilder& sb) {
	int parent = 0;
	p = psnum(skipsp(p), parent);
	auto pe = bsdata<talkei>::add(); pe->clear();
	pe->index = parent;
	p = read_variants(skipsp(p), pe->elements, sb);
	p = skipspcr(skipline(p));
	if(isevent(p))
		return p;
	p = read_string(p, pe->text, sb);
	while(isanswer(p))
		p = read_answer(p + 1, sb, pe);
	return p;
}

void read_talk(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	szfnamewe(temp, url);
	auto current_talk = bsdata<talki>::add();
	memset(current_talk, 0, sizeof(*current_talk));
	current_talk->id = szdup(temp);
	current_talk->elements.setbegin();
	while(*p) {
		if(!isevent(p)) {
			log::errorp(p, "Expected symbol `#` followed by event number");
			break;
		}
		p = read_event(p + 1, sb);
	}
	log::close();
	current_talk->elements.setend();
}

void talkei::clear() {
	memset((void*)this, 0, sizeof(*this));
}

talki* find_talk(const talkei* p) {
	for(auto& e : bsdata<talki>()) {
		auto pb = e.elements.begin();
		auto pe = e.elements.end();
		if(p >= pb && p < pe)
			return &e;
	}
	return 0;
}

talkei* find_speech(const talki* current_talk, int id) {
	if(!current_talk)
		return 0;
	for(auto& e : current_talk->elements) {
		if(e.answer())
			continue;
		if(e.index != id)
			continue;
		//if(!script_run(e.elements, false))
		//	continue;
		return &e;
	}
	return 0;
}

bool simple_replic(const talkei* p) {
	auto pt = find_talk(p);
	if(!pt)
		return true;
	auto pe = pt->elements.end();
	if(p + 1 == pe)
		return true;
	return !p[1].answer();
}