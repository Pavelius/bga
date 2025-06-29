#include "nameable.h"
#include "tag.h"

#pragma once

struct variable : nameable {
	char		lock_difficult, trap_difficult, counter, stage;
	unsigned	flags;
	bool		is(tagn v) const { return (flags & (1 << v)) != 0; }
	void		remove(tagn v) { flags &= ~(1 << v); }
	void		set(tagn v) { flags |= (1 << v); }
};
extern variable* current_variable_base;
extern unsigned variable_count;
struct variableid {
	short unsigned variable_index;
	variable&	getvar() const { return current_variable_base[variable_index]; }
};