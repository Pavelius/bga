#include "actable.h"
#include "bsdata.h"
#include "stringbuilder.h"

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

void act_identifier(stringbuilder& sb, const char* identifier) {
	if(actor) {
		if((equal(identifier, "�����") || equal(identifier, "name"))) {
			sb.add(actor->getname());
			return;
		} else if(equal(identifier, "�����")) {
			sb.addof(actor->getname());
			return;
		}
		else if(equal(identifier, "�����")) {
			sb.addto(actor->getname());
			return;
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
				return;
			}
		}
	}
	default_string(sb, identifier);
}

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, char separator) const {
	auto push_actor = actor; actor = this;
	sb.addv(format, format_param);
	actor = push_actor;
}