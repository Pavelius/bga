#include "nameable.h"
#include "stringbuilder.h"

#pragma once

struct stringvari : nameable {
	fnprint		proc;
};

bool stringvar_identifier(stringbuilder& sb, const char* identifier);