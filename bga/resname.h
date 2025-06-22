#pragma once

struct resname {
	char name[32];
	explicit operator bool() const { return name[0] != 0; }
	operator const char*() const { return name; }
	bool operator==(const char* v) const;
	void clear() { name[0] = 0; }
	void set(const char* p);
	const char* getname() const;
};
int compare_resname_name(const void* v1, const void* v2);