#pragma once

constexpr unsigned fg(unsigned v) { return 1 << v; }
template<typename... Args>
constexpr unsigned fg(unsigned v, Args... args) { return fg(v) | fg(args...); }

// Abstract flag data bazed on enumerator
template<typename T, typename DT = unsigned>
class cflags {
	DT data = 0;
public:
	constexpr cflags() : data(0) {}
	constexpr cflags(const T v) : data(v) {}
	template<typename... Args>constexpr cflags(Args... args) : data(fg(args...)) {}
	constexpr explicit operator bool() const { return data != 0; }
	constexpr void add(const T v) { data |= 1 << v; }
	constexpr void add(const cflags& e) { data |= e.data; }
	constexpr void clear() { data = 0; }
	constexpr bool is(const T id) const { return (data & (1 << id)) != 0; }
	constexpr bool allof(const cflags& e) const { return (data & e.data) == data; }
	constexpr bool is(const cflags& e) const { return (data & e.data) != 0; }
	constexpr void remove(T id) { data &= ~(1 << id); }
};


