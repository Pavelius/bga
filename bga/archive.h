#include "adat.h"
#include "array.h"
#include "io_stream.h"

#pragma once

// Fast and simple driver for streaming binary data.
// Allow arrays and simple collections.
struct archive {
	iostream&	source;
	bool		writemode;
	constexpr archive(iostream& source, bool writemode) : source(source), writemode(writemode) {}
	bool		signature(const char* id);
	bool		signature(unsigned long value);
	bool		version(short major, short minor);
	void		set(void* value, unsigned size);
	void		setpointer(array& source, void** value);
	// Array with fixed count
	template<typename T, size_t N> void set(T(&value)[N]) {
		for(int i = 0; i < N; i++)
			set(value[i]);
	};
	// Fixed data collection
	template<typename T, unsigned N> void set(adat<T, N>& value) {
		set(value.count);
		for(auto& e : value)
			set(e);
	}
	// All simple types and requisites
	template<class T> void set(T& value) {
		set(&value, sizeof(value));
	}
	template<class T> void setc(array& v) {
		if(writemode) {
			set(v.count);
		} else {
			decltype(v.count) count;
			set(count);
			v.reserve(count);
			v.setcount(count);
		}
		auto pe = v.end();
		for(auto p = v.begin(); p < pe; p += v.element_size)
			set(*((T*)p));
	}
};
template<> void archive::set<array>(array& v);
template<> void archive::set<const char*>(const char*& v);
