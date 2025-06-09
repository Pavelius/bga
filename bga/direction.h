#pragma once

enum directionn : unsigned char {
	Center,
	Left, Up, Right, Down,
	LeftUp, RightUp, RightDown, LeftDown
};
directionn to(directionn d, directionn dm);
