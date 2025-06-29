#pragma once

#include "variant.h"

struct keybind {
	const char* key_string;
	unsigned	key;
	variant		command; // Can be script, list or form
};

void apply_keybinding();
void initialize_keybind();
keybind* find_key(unsigned key);