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
	void	setorigin(int v);
};
