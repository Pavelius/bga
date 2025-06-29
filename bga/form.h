#pragma once

#include "nameable.h"

typedef void(*fnevent)();

struct form : nameable {
	fnevent	command;
	long	param1, param2;
	void*	object;
};
