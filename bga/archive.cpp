#include "archive.h"
#include "stringbuilder.h"

bool archive::signature(const char* id) {
	char temp[4];
	if(writemode) {
		memset(temp, 0, sizeof(temp));
		stringbuilder sb(temp); sb.add(id);
		set(temp, sizeof(temp));
	} else {
		set(temp, sizeof(temp));
		if(szcmpi(temp, id) != 0)
			return false;
	}
	return true;
}

bool archive::signature(unsigned long value) {
	unsigned long v2 = 0;
	if(writemode)
		set(value);
	else {
		set(v2);
		if(value != v2)
			return false;
	}
	return true;
}

bool archive::version(short major, short minor) {
	short major_reader = major;
	short minor_reader = minor;
	set(major_reader);
	set(minor_reader);
	if(!writemode) {
		if(major_reader < major)
			return false;
		else if(major_reader == major && minor_reader < minor)
			return false;
	}
	return true;
}

template<> void archive::set<array>(array& v) {
	set(v.count);
	set(v.element_size);
	if(!writemode)
		v.reserve(v.count);
	set(v.data, v.element_size * v.count);
}

void archive::set(void* value, unsigned size) {
	if(writemode)
		source.write(value, size);
	else
		source.read(value, size);
}