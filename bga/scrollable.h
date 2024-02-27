#pragma once

struct scrollable {
	int		origin, maximum, perscreen, perline;
	scrollable() : origin(0), maximum(-1), perscreen(0), perline(0) {}
	void	clear();
	void	correct();
	void	input();
	bool	isinvalidated() const { return maximum == -1; }
	virtual void invalidate() { maximum = -1; }
	void	keyinput();
	void	view_down() { setcommand(origin + perline); }
	void	view_up() { setcommand(origin - perline); }
	void	paint();
	int		proportial(int max_height) const;
	void	setcommand(int v);
	void	setorigin(int v);
	virtual void update() {}
};
class scrolltext : public scrollable {
	int		cashe_origin = 0, cashe_content = -1, current_origin = -1;
public:
	void	invalidate();
	void	paint(const char* format);
};
class scrolllist : public scrollable {
};
extern scrollable* last_scroll;