#pragma once

struct namestr {
	char name[32];
	constexpr explicit operator bool() const { return name[0] != 0; }
	void setname(const char* format, ...);
};
