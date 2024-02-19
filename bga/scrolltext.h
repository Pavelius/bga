#pragma once

struct scrollable {
	int		origin, current, maximum, perscreen, perline;
	void	clear();
	void	correct();
	void	input();
	virtual void invalidate() = 0;
	void	keyinput();
	void	view_down() { setcommand(origin + perline); }
	void	view_up() { setcommand(origin - perline); }
	int		proportial(int max_height) const;
	void	setcommand(int v);
	void	setorigin(int v);
};
class scrolltext : public scrollable {
	int		cashe_origin = 0, cashe_content = -1;
public:
	void	invalidate();
	void	paint(const char* format);
};
class scrolllist : public scrollable {
public:
	void	invalidate() {}
};
extern scrollable* last_scrolltext;