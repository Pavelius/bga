#pragma once

class scrolltext {
	int		cashe_origin, cashe_string, maximum;
	int		origin, perscreen;
public:
	void	clear();
	void	correct();
	void	input();
	void	invalidate();
	void	paint(const char* format);
	int		proportial(int max_height) const;
	void	setorigin(int v);
};
extern scrolltext* last_scrolltext;