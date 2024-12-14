#include "tag.h"

#pragma once

struct variable {
	char		lock_difficult, trap_difficult;
	unsigned	flags;
	bool		is(tag_s v) const { return (flags & (1 << v)) != 0; }
	void		remove(tag_s v) { flags &= ~(1 << v); }
	void		set(tag_s v) { flags |= (1 << v); }
};
extern variable* current_variable_base;
extern unsigned variable_count;
struct variableid {
	short unsigned variable_index;
	variable&	getvar() const { return current_variable_base[variable_index]; }
};
