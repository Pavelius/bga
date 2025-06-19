#include "adat.h"
#include "stringbuilder.h"

#pragma once

class answers {
	struct element {
		const void* value;
		const char* text;
	};
	char				buffer[1024];
	stringbuilder		sc;
	adat<element, 24>	elements;
public:
	typedef void(*fnpaint)(int index, const void* value, const char* text, fnevent press_event);
	answers() : sc(buffer) {}
	constexpr operator bool() const { return elements.count != 0; }
	void				add(const void* value, const char* name, ...);
	void				addv(const void* value, const char* name, const char* format);
	const element*		begin() const { return elements.data; }
	element*			begin() { return elements.data; }
	void*				choose(const char* title = 0, const char* cancel_text = 0, int cancel_mode = 0) const;
	void				clear();
	static int			compare(const void* v1, const void* v2);
	const element*		end() const { return elements.end(); }
	const char*			getname(void* v);
	int					indexof(const void* v) const { return elements.indexof(v); }
	void				paintanswers(int columns, const char* cancel_text) const;
	void*				random() const;
	void				remove(int index) { elements.remove(index, 1); }
	size_t				size() const { return elements.size(); }
	void				sort();
};
extern answers an;