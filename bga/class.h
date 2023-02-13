#pragma once

enum class_s : unsigned char {
	Commoner,
	Fighter, Cleric, Rogue, Sorcerer, Wizard,
};
struct classi {
	const char*		id;
};
struct classa {
	char			classes[Wizard + 1];
	class_s			getmainclass() const;
};
