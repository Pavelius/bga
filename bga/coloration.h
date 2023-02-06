#pragma once

struct coloration {
	unsigned char			skin;
	unsigned char			hair;
	unsigned char			minor;
	unsigned char			major;
	unsigned char			metal;
	unsigned char			leather;
	unsigned char			armor;
	//
	explicit operator bool() const { return skin == hair == minor == major == 0; }
	//constexpr coloration() : skin(0), hair(0), minor(0), major(0), armor(/*28*/MetalSteel), metal(MetalIron), leather(23) {}
	void					set(short unsigned portrait);
	//void					upload(color* col) const;
};
