#include "crt.h"
#include "stringbuilder.h"

#pragma once

#define VAR(T, K) bsmeta<T>::meta, bsdata<T>::source_ptr, K

struct bsreq;
union variant;

typedef sliceu<variant> variants;
typedef void (*fngetinfo)(const void* object, variant v, stringbuilder& sb);
template<typename T>
void ftstatus(const void* object, stringbuilder& sb);

struct varianti {
	typedef void(*fnscript)(int index, int bonus);
	typedef bool(*fntest)(int index, int bonus);
	const char*		id;
	const bsreq*	metadata;
	array*			source;
	int             key_count;
	fnstatus		pgetinfo;
	fngetinfo		pgetproperty;
	fnscript		pscript;
	fntest			ptest;
	static const varianti* find(const void* object);
	void			set(void* object, const char* id, void* value) const;
	void			set(void* object, const char* id, int value) const;
};
union variant {
	unsigned char uc[4];
	unsigned u;
	struct {
		unsigned short value;
		char counter;
		unsigned char type;
	};
	constexpr variant() : u(0) {}
	constexpr variant(unsigned char t, unsigned short n) : value(n), counter(0), type(t) {}
	constexpr variant(unsigned char t, unsigned short n, char c) : value(n), counter(c), type(t) {}
	constexpr variant(int u) : u(u) {}
	template<class T> variant(T* v) : variant((const void*)v) {}
	constexpr explicit operator unsigned() const { return u; }
	constexpr explicit operator bool() const { return u != 0; }
	constexpr bool operator==(const variant& v) const { return u == v.u; }
	constexpr bool operator!=(const variant& v) const { return u != v.u; }
	template<class T> operator T*() const { return (T*)((bsdata<varianti>::elements[type].source == bsdata<T>::source_ptr) ? getpointer() : 0); }
	void			clear() { u = 0; }
	constexpr bool	issame(const variant& v) const { return type == v.type && value == v.value; }
	template<class T> constexpr bool iskind() const { return bsdata<varianti>::elements[type].source == bsdata<T>::source_ptr; }
	const varianti& geti() const { return bsdata<varianti>::elements[type]; }
	const char*		getid() const;
	void			getinfo(stringbuilder& sb) const;
	void*			getpointer() const { return geti().source->ptr(value); }
	const char*		getname() const;
	void			setvariant(unsigned char t, unsigned short v) { type = t; value = v; counter = 0; }
};
template<> variant::variant(const char* v);
template<> variant::variant(const void* v);