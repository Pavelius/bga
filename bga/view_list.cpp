#include "view_list.h"

void correct_table(int& origin, int maximum, int per_page) {
	if(origin + per_page > maximum)
		origin = maximum - per_page;
	if(origin < 0)
		origin = 0;
}