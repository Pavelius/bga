#pragma once

struct nameable {
	const char*	id;
	explicit operator bool() const { return id != 0; }
	const char*	getid() const { return id; }
	const char*	getname() const;
	const char*	getshortname() const;
	static const char* getname(const void* object) { return ((nameable*)object)->getname(); }
};

int compare_nameable(const void* v1, const void* v2);