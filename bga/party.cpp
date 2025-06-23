#include "alignment.h"
#include "bsdata.h"
#include "console.h"
#include "class.h"
#include "gender.h"
#include "io_stream.h"
#include "log.h"
#include "party.h"
#include "race.h"
#include "stringbuilder.h"

using namespace log;

static const char* p;
static char text_value[4096];
static stringbuilder sb(text_value);
partyi* last_party;

void partyi::clear() {
	if(content)
		delete[] content;
	memset(this, 0, sizeof(*this));
}

void partyi::release() {
	if(content)
		delete[] content;
}

static void skip(const char* sym) {
	auto i = 0;
	while(sym[i]) {
		if(sym[i] != p[i]) {
			errorp(p, "Expected `%1`", sym);
			return;
		}
		i++;
	}
	p = skipsp(p + i);
}

static void skipne(char sym) {
	if(*p == sym)
		p = skipsp(p + 1);
}

static void skip(char sym) {
	if(*p != sym) {
		char sym_text[2] = {sym, 0};
		errorp(p, "Expected symbol %1", sym_text);
	} else
		p = skipsp(p + 1);
}

static void parse_type(partyi::character& e) {
	if(p[0] != '(')
		return;
	p = skipsp(p + 1);
	while(*p) {
		if(*p == ')') {
			p = skipsp(p + 1);
			break;
		}
		sb.clear();
		p = skipsp(sb.psidf(p));
		variant v = sb.begin();
		if(v.iskind<genderi>())
			e.gender = (gendern)v.value;
		else if(v.iskind<racei>())
			e.race = (racen)v.value;
		else if(v.iskind<classi>())
			e.type = (classn)v.value;
		else if(v.iskind<alignmenti>())
			e.alignment = (alignmentn)v.value;
		else
			errorp(p, "Unknown character type `%1`", sb.begin());
	}
	skip(":");
}

static int parse_number() {
	int number = 0;
	p = psnum(p, number);
	return number;
}

static bool token(const char* id) {
	if(p[0] != ':')
		return false;
	auto len = zlen(id);
	if(memcmp(id, text_value, len) != 0)
		return false;
	if(text_value[len] != 0)
		return false;
	p = skipsp(p + 1);
	return true;
}

static void parse_feats(partyi::character& e) {
	while(*p && *p != 10 && *p != 13) {
		if(*p == '.') {
			p = skipsp(p + 1);
			break;
		}
		sb.clear();
		p = skipsp(sb.psidf(p));
		auto ps = bsdata<feati>::find(text_value);
		if(!ps)
			errorp(p, "Not found feat `%1`", text_value);
		else
			e.feats.set(ps->getindex());
		skipne(',');
	}
}

static void parse_skills(partyi::character& e) {
	while(*p && *p != 10 && *p != 13) {
		if(*p == '.') {
			p = skipsp(p + 1);
			break;
		}
		sb.clear();
		auto pn = skipsp(sb.psidf(p));
		if(*pn != '(')
			break;
		p = pn;
		auto ps = bsdata<skilli>::find(text_value);
		skipne('(');
		if(!ps) {
			errorp(p, "Not found skill `%1`", text_value);
			e.skills[0] = (char)parse_number();
		} else
			e.skills[ps->getindex()] = (char)parse_number();
		skip(')');
		skipne(',');
	}
}

static void parse_stats(partyi::character& e) {
	while(*p && *p != 10 && *p != 13) {
		if(*p == '.') {
			p = skipsp(p + 1);
			break;
		}
		sb.clear();
		p = skipsp(sb.psidf(p));
		if(token("STR"))
			e.abilities[0] = (char)parse_number();
		else if(token("DEX"))
			e.abilities[1] = (char)parse_number();
		else if(token("CON"))
			e.abilities[2] = (char)parse_number();
		else if(token("INT"))
			e.abilities[3] = (char)parse_number();
		else if(token("WIS"))
			e.abilities[4] = (char)parse_number();
		else if(token("CHA"))
			e.abilities[5] = (char)parse_number();
		else if(token("PT"))
			e.portrait = (short unsigned)parse_number();
		else if(token("Feats"))
			parse_feats(e);
		else if(token("Skills"))
			parse_skills(e);
		if(*p == ',')
			p = skipsp(p + 1);
	}
}

static void parse_word() {
	sb.clear();
	while(*p) {
		if(*p == 10 || *p == 13 || *p=='(' || *p==':')
			break;
		else
			sb.addch(*p++);
	}
	sb.trimr();
}

static void parse_character(partyi::character& e) {
	// Character name
	sb.clear();
	p = skipsp(sb.psidf(p));
	e.name.set(sb);
	parse_word();
	e.nickname.set(sb);
	parse_type(e);
	parse_stats(e);
}

static void parse_party() {
	// Add party name (required, filled line #1)
	sb.clear();
	p = skipspcr(sb.psstrlf(skipspcr(p)));
	add_locale(last_party->id, sb);
	last_party->description = p;
	// Add party description (required, filled line #2)
	sb.clear();
	p = skipspcr(sb.psstrlf(skipspcr(p)));
	add_locale(ids(last_party->id, "Info"), sb);
	// Characters
	for(auto i = 0; i < lenghtof(last_party->characters) && p[0] == '-'; i++) {
		auto& e = last_party->characters[i];
		p = skipws(p + 1); // Skip '-'
		parse_character(e);
		p = skipspcr(p);
	}
}

void read_party(const char* url) {
	auto pf = log::read(url, true);
	if(!pf)
		return;
	last_party = bsdata<partyi>::add();
	last_party->id = szdup(szfnamewe(text_value, url));
	last_party->content = pf;
	p = pf;
	parse_party();
}

void initialize_parties() {
	readf(read_party, "parties", "*.txt");
	update_locale_names();
}