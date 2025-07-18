#pragma once

struct rect {
	int		x1, y1;
	int		x2, y2;
	explicit operator bool() const { return x1 != 0 || x2 != 0 || y1 != 0 || y2 != 0; }
	bool	operator!=(const rect& rc) const { return rc.x1 != x1 || rc.x2 != x2 || rc.y1 != y1 || rc.y2 != y2; }
	bool	operator==(const rect& rc) const { return rc.x1 == x1 && rc.x2 == x2 && rc.y1 == y1 && rc.y2 == y2; }
	rect	operator+(const rect& rc) const { return{x1 + rc.x1, y1 + rc.y1, x2 + rc.x2, y2 + rc.y2}; }
	rect	operator+(const int v) const { return{x1 + v, y1 + v, x2 - v, y2 - v}; }
	rect	operator-(const int v) const { return{x1 - v, y1 - v, x2 + v, y2 + v}; }
	int		bottom() const { return y1 > y2 ? y1 : y2; }
	void	clear() { x1 = x2 = y1 = y2 = 0; }
	int		cx() const { return x1 + (x2 - x1) / 2; }
	int		cy() const { return y1 + (y2 - y1) / 2; }
	int		height() const { return y2 - y1; }
	bool	intersect(const rect& rc) const { return x1<rc.x2 && x2>rc.x1 && y1<rc.y2 && y2>rc.y1; }
	int		left() const { return x1 < x2 ? x1 : x2; }
	void	move(int x, int y) { x1 += x; x2 += x; y1 += y; y2 += y; }
	void	offset(int x, int y) { x1 += x; x2 -= x; y1 += y; y2 -= y; }
	void	offset(int n) { offset(n, n); }
	int		right() const { return x1 > x2 ? x1 : x2; }
	void	set(int px1, int py1, int px2, int py2) { x1 = px1; x2 = px2; y1 = py1; y2 = py2; }
	int		size() const { return (width() > height()) ? width() : height(); }
	int		top() const { return y1 < y2 ? y1 : y2; }
	int		width() const { return x2 - x1; }
};