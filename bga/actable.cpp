#include "actable.h"
#include "bsdata.h"
#include "console.h"
#include "gender.h"
#include "stringbuilder.h"
#include "stringvar.h"

namespace {
struct gender_change_string {
	const char*	female;
	const char*	male;
	const char*	multiply;
};
static gender_change_string player_gender[] = {
	{"�������", "��������", "�������"},
	{"�������", "�������", "�������"},
	{"������", "�������", "�������"},
	{"������", "������", "�������"},
	{"����", "����", "�����"},
	{"�����", "�����", "�����"},
	{"���", "��", "���"},
	{"���", "���", "���"},
	{"���", "����", "���"},
	{"���", "��", "���"},
	{"��", "��", "��"},
	{"��", "���", "��"},
	{"��", "", "��"},
	{"��", "���", "��"},
	{"�", "", "�"},
};
}

static const actable* actor;

static bool actor_identifier(stringbuilder& sb, const char* identifier) {
	if(!actor)
		return false;
	if((equal(identifier, "�����") || equal(identifier, "name"))) {
		sb.add(actor->getname());
		return true;
	} else if(equal(identifier, "�����")) {
		sb.addof(actor->getname());
		return true;
	} else if(equal(identifier, "�����")) {
		sb.addto(actor->getname());
		return true;
	} else {
		for(auto& e : player_gender) {
			if(equal(e.female, identifier) != 0)
				continue;
			if(actor->gender == NoGender)
				sb.add(e.multiply);
			else if(actor->gender == Female)
				sb.add(e.female);
			else
				sb.add(e.male);
			return true;
		}
	}
	return false;
}

void act_identifier(stringbuilder& sb, const char* identifier) {
	if(actor_identifier(sb, identifier))
		return;
	if(stringvar_identifier(sb, identifier))
		return;
	default_string(sb, identifier);
}

void actable::actv(const char* format, const char* format_param, const char* line_feed) const {
	auto push_actor = actor; actor = this;
	printcnv(format, format_param, line_feed);
	actor = push_actor;
}