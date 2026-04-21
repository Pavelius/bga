#pragma once

#include "bsdata.h"
#include "nameable.h"

template<typename T>
struct enumi : nameable {
};

#define assert_enum(last) static_assert(sizeof(bsdata<enumi<decltype(last)>>::elements) / sizeof(bsdata<enumi<decltype(last)>>::elements[0]) == static_cast<int>(last) + 1, "Invalid count of " #last " type elements"); BSDATAF(enumi<decltype(last)>)

// Export function to get id of enumaration value. Application must define all calls.
template<typename T> inline const char* enumid(T v) { return bsdata<enumi<T>>::elements[v].id; }

template<typename T> inline T enumn(const enumi<T>* v) { return (T)(v - bsdata<enumi<T>>::elements); }
