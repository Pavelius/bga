#include "variant.h"

#pragma once

template<typename T>
void fnscript(int index, int bonus);

template<typename T>
bool fntest(int index, int bonus);

struct script {
	typedef void(*fnrun)(int bonus);
	typedef bool(*fntest)(int bonus);
	const char*	id;
	fnrun		proc;
	fntest		test;
	static bool allow(variant v);
	static bool allow(const variants& source);
	static void run(variant v);
	static void run(const char* id, int bonus);
	static void run(const variants& source);
};
extern bool stop_script;