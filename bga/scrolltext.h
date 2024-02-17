#pragma once

class scrolltext {
	int		cashe_origin, cashe_string, maximum;
	int		origin, perscreen, text_height;
	void	setcommand(int v);
public:
	void	clear();
	void	correct();
	void	input();
	void	invalidate();
	void	move_down() { setcommand(origin + text_height); }
	void	move_up() { setcommand(origin - text_height); }
	void	paint(const char* format);
	int		proportial(int max_height) const;
	void	setorigin(int v);
};
extern scrolltext* last_scrolltext;