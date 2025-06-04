#pragma once

typedef void (*fnevent)();

enum resn : unsigned short;

struct item;

extern item *drag_item_source, *drag_item_dest;

void button(resn res, unsigned short f1, unsigned short f2, unsigned key = 0);
void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id);
void button_check(unsigned key);
void change_zoom_factor();
void fire(fnevent proc, long param = 0, long param2 = 0, const void* object = 0);
void hotkey(unsigned key, fnevent proc, int param = 0);
void invalidate_description();
void paint_action_panel();
void paint_area();
void paint_dialog(resn v);
void paperdoll();
long open_dialog(fnevent proc, bool faded);
void open_item_description();
void open_game();
void update_creature();

unsigned char open_color_pick(unsigned char current_color, unsigned char default_color);
