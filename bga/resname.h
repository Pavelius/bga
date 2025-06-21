#pragma once

struct resname {
	char name[12];
	bool operator==(const char* v) const;
	void clear() { name[0] = 0; }
	void set(const char* p);
	const char* getname() const;
};
int compare_resname_name(const void* v1, const void* v2);